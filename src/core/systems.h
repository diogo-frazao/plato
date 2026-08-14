#pragma once

#include "lib.h"
#include "constants.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include <stdint.h>
#include "text.h"

class Level;
class MovementComponent;
class TransformComponent;
class AttackingComponent;
class SpriteComponent;
class RectColliderComponent;
class Entity;
class SDL_Texture;

// This needs to be the very first system to run, to ensure positions are saved for rendering interpolation
// For more information, read Gaffer Fix Your Timestep
class SavePreviousPositionSystem
{
public:
	void update();
};

class RenderingSystem
{
public:
	void createLightsBuffers();
	void render(float renderAlpha);
	void renderCrosshair(float renderAlpha);

	SDL_Color _targetAmbientColor = { 255, 255, 255, 255 };
	// FColor since we use float lerping for changing the ambient color at runtime
	SDL_FColor _currentAmbientColor = { 0, 0, 0, 0 };
	// Used only for debugging with imgui
	float _debugAmbientColorPicker[4] = {255, 255, 255};

	void setTargetAmbientColor(uint8_t r, uint8_t g, uint8_t b)
	{
		_targetAmbientColor.r = r;
		_targetAmbientColor.g = g;
		_targetAmbientColor.b = b;
		_targetAmbientColor.a = 255;

#ifndef RELEASE_BUILD
		_debugAmbientColorPicker[0] = _targetAmbientColor.r / 255.f;
		_debugAmbientColorPicker[1] = _targetAmbientColor.g / 255.f;
		_debugAmbientColorPicker[2] = _targetAmbientColor.b / 255.f;
#endif // !RELEASE_BUILD
	}

	SDL_Texture* loadAtlas(AtlasType type);
	void reloadAtlas(AtlasType type);
private:
	SDL_Texture* getTargetLightsBuffer(LayerType layer);

	void computeLightsAtLayer(LayerType layer, bool isAffectedByAmbientLight = false);
	void renderSpritesAtLayer(LayerType layer, float renderAlpha);
	void renderLightsAtLayer(LayerType layer, bool isAffectedByAmbientLight = false);

	SDL_FRect _src;
	SDL_FRect _dest;

	SDL_Texture* _loadedAtlasFiles[k_maxAtlasFiles]{ nullptr, nullptr };
	SDL_Texture* _backLightsBuffer = nullptr;
	SDL_Texture* _frontLightsBuffer = nullptr;
};

class AnimationSystem
{
public:
	void update();
};

class MovementSystem
{
public:
	void update();

	// General functions
	void processHorizontalMovement(Entity* self);
	void processVerticalMovement(Entity* self);
	bool willCollideWithLevelGeometryAtPosition(Entity* self, const Vec2 positionToCheck);

	// Main character specific
	void processMainCharacterMovement();
	void handleCoyoteTime(MovementComponent* movementComponent, bool wasGrounded);
};

class DebugSystem
{
public:
	void render();
	static void debugRect(Vec2 position, RectCollider collider, SDL_Color color, bool isScreenSpaceRect = false);
	void debugLine(Vec2 start, Vec2 end, SDL_Color color);
	void debugPoint(Vec2 position, SDL_Color color);
};

struct CrossHairSmear
{
	Entity* entity = nullptr;
	Vec2 directionToMove = { 0,0 };
	float moveSpeed = 4.f;
};

class CrosshairSystem
{
public:
	void update();

	inline static float _resetSpritetimer = k_invalidTime;
	static void crosshairMeleeHitFeedback(Vec2 hitLocation);

	inline static uint8_t s_corsshairOpacity = 255;
	inline static CrossHairSmear _crosshairSmear;
};

class AttackingSystem
{
public:
	void update();
	
	// Player attacks
	void handleMainCharacter();
	void tryMainCharacterAttack(Entity* player, AttackingComponent* a, MovementComponent* m, TransformComponent* t, SpriteComponent* s, RectColliderComponent* c);
	void handleMainCharacterAttackAnimations(Entity* player, AttackingComponent* a, MovementComponent* m, SpriteComponent* s);

	bool hasPlayerAlreadyAttackedEntity(int32_t entityId);
	void registerPlayerAttackToEntity(Entity* entity);
	void clearEntitiesPlayerAttacked();
	int32_t _entitiesPlayerAttackedForCurrentAttack[10];

	// Cleared at the beginning of each frame
	// This is the only place where we use RectCollider.topLeftPointOffset as a world position
	inline static RectCollider s_attackCollisionsToDebugThisFrame[10];

	void addColliderToDebugList(Vec2 position, RectCollider collider)
	{
#ifndef RELEASE_BUILD
		for (RectCollider& col : s_attackCollisionsToDebugThisFrame)
		{
			if (!col.isValidCollider())
			{
				Vec2 colliderStartingPosition = getColliderPosition(position, collider);
				col.topLeftPointOffset = { (int32_t)colliderStartingPosition.x, (int32_t)colliderStartingPosition.y };
				col.size = collider.size;
				return;
			}
		}

		D_LOG(ERROR, "addColliderToDebugList(): Colliders to debug is full, can't debug more");

#endif // !RELEASE_BUILD
	}

	void clearDebugCollisions()
	{
		for (RectCollider& col : s_attackCollisionsToDebugThisFrame)
		{
			col.invalidate();
		}
	}
};

enum DialogueAlignmentType
{
	DIALOGUE_CENTER_ALIGNED,
	DIALOGUE_LEFT_ALIGNED,
	DIALOGUE_RIGHT_ALIGNED
};

class UISystem
{
public:
	void start();
	void update();
	void render(RenderingSystem* renderingSystem);
	void debugColliders();

	struct DialogueCharacter
	{
		IVec2 atlasOffset{ 0,0 };
		Vec2 size{ 0.f, 0.f };
		Vec2 startingPosition{ 0.f, 0.f };
		float secondsToStartShowingCharacter = 0.f;
		SDL_Color overrideColor = { 0, 0, 0, 0 };
		TextEffectType textEffectToApply = INVALID_EFFECT;

		// Changed at runtime if needed
		Vec2 position{ 0.f, 0.f };
		float opacity = 0.f;
		float dynamicYSize = 0.f;
		Vec2 velocity = { 0.f, 0.f };
		float timeSinceCharacterAppeared = 0.f;
		// Used to run code only once per character
		bool wasInitialized = false;
		float angularVelocity = 0.f;
		float rotationAngle = 0.f;

		bool isValid()
		{
			return size.x > 0.f;
		}

		void reset()
		{
			this->atlasOffset = { 0,0 };
			this->startingPosition = { 0.f, 0.f };
			this->position = { 0.f, 0.f };
			this->size = { 0.f, 0.f };
			this->secondsToStartShowingCharacter = 0.f;
			this->opacity = 0.f;
			this->dynamicYSize = 0.f;
			this->velocity = { 0.f, 0.f };
			this->overrideColor = { 0, 0,0 };
			this->timeSinceCharacterAppeared = 0.f;
			this->textEffectToApply = INVALID_EFFECT;
			this->wasInitialized = false;
			this->rotationAngle = 0.f;
			this->angularVelocity = 0.f;
		}
	};

	enum DialogueOptionState
	{
		DIALOGUE_OPTION_IDLE_STATE,
		DIALOGUE_OPTION_HOVERED_STATE,
		DIALOGUE_OPTION_CHOSEN_STATE,
		DIALOGUE_OPTION_NOT_CHOSEN_STATE
	};

	struct DialogueOption
	{
		DialogueCharacter characters[k_maxCharactersPerDialogue];
		DialogueOptionState state = DIALOGUE_OPTION_IDLE_STATE;
		TextType dialogueType = INVALID_TEXT;
		int8_t tensionDelta = 0;
		TextTensionType optionTensionType = NORMAL_TENSION;

		// Used to know x,y,w,h for collisions
		SDL_FRect colliderDest{};

		// Changed at runtime
		Vec2 dialogueBoxSize{ 0.f, 0.f };
		float fadeOutTimer = 0.f;
		float dialogueBoxDynamicYSize = 0.f;
		float secondsToStartShowingOption = 0.f;
		// Used to fade out when another option is selected. 
		// This controls the opacity of everything rendered for a dialogue option (text + sprites)
		float opacity = 255.f;
		// Color of the square background that surrounds characters
		SDL_Color backgroundSpriteColor{ 0, 0, 0, 0};
		// Color of the hovered sprite
		SDL_Color hoveredBorderSpriteColor{ 0, 0, 0, 0 };

		void destroyDialogueOption()
		{
			for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
			{
				if (!characters[i].isValid())
				{
					break;
				}

				this->characters[i].reset();
			}

			this->dialogueBoxSize = {0.f, 0.f};
			this->colliderDest.x = this->colliderDest.y = this->colliderDest.w = this->colliderDest.h = 0.f;
			this->dialogueType = INVALID_TEXT;
			this->state = DIALOGUE_OPTION_IDLE_STATE;
			this->opacity = 255.f;
			this->backgroundSpriteColor = { 0, 0, 0, 0 };
			this->hoveredBorderSpriteColor = { 0, 0, 0, 0};
			this->fadeOutTimer = 0.f;
			this->dialogueBoxDynamicYSize = 0.f;
			this->secondsToStartShowingOption = 0.f;
			this->tensionDelta = 0;
			this->optionTensionType = NORMAL_TENSION;
		}

		bool isValid()
		{
			return characters[0].isValid();
		}
	};

	enum DialogueStateType
	{
		DIALOGUE_BASE_STATE, // Base state unless it's changed for one of the below
		DIALOGUE_ENDED_STATE, // Set automatically when a dialogue finishes or we choose an option. 
		DIALOGUE_INTERRUPTED_STATE, // Set manually. Will also end the dialogue but in a different way
		DIALOGUE_FINISHED_INTERRUPTED, // Set automatically when the interrupted animation finishes
	};

	struct Dialogue
	{
		DialogueCharacter characters[k_maxCharactersPerDialogue];
		uint8_t tensionDelta = 0;

		// Changed at runtime
		TextType dialogueType = INVALID_TEXT;
		DialogueStateType state = DIALOGUE_BASE_STATE;
		bool isScreenSpace = false;
		float timeSinceDialogueStarted = 0.f;
		float timeSinceFinalCharacterWasDrawn = 0.f;
		Vec2 dialogueBoxSize{ 0.f, 0.f };
		Vec2 topLeftPosition{ 0.f,0.f };
		DialogueAlignmentType alignmentType = DIALOGUE_CENTER_ALIGNED;
		// Used to animate the dialogue box from left to right
		float dialogueBoxDynamicXSize = 0.f;
		float speechIndicatorDynamicYSize = 0.f;
		// Only matters if the current dialogue has options
		TextType dialogueOptionChosen = INVALID_TEXT;
		Entity* entityTalking = nullptr;
		float dialogueOutlineDynamicXSize = 0.f;
		// Used since text effects that cause a camera shake should only shake once on startup. Not once per character
		bool hasAppliedShakeForCurrentWord = false;
		// Needed to have frame rate independent lerp
		float speechIndicatorCurrentXPosition = 0.f;
		float speechIndicatorTargetXPosition = 0.f;

		// Don't use directly, call destroyCurrentDialogue() instead
		void destroyDialoge()
		{
			for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
			{
				if (!characters[i].isValid())
				{
					break;
				}

				this->characters[i].reset();
			}

			this->state = DIALOGUE_BASE_STATE;
			this->timeSinceDialogueStarted = 0.f;
			this->timeSinceFinalCharacterWasDrawn = 0.f;
			this->dialogueBoxSize = { 0.f, 0.f };
			this->isScreenSpace = false;
			this->alignmentType = DIALOGUE_CENTER_ALIGNED;
			this->dialogueBoxDynamicXSize = 0.f;
			this->dialogueType = INVALID_TEXT;
			this->dialogueOptionChosen = INVALID_TEXT;
			this->tensionDelta = 0;
			this->entityTalking = nullptr;
			this->speechIndicatorDynamicYSize = 0.f;
			this->dialogueOutlineDynamicXSize = 0.f;
			this->hasAppliedShakeForCurrentWord = false;
			this->speechIndicatorCurrentXPosition = 0.f;
			this->speechIndicatorTargetXPosition = 0.f;
		}
	};

	struct DialogueOptionsDTO
	{
		TextType options[k_maxDialogueOptions];

		DialogueOptionsDTO(TextType opt1 = INVALID_TEXT, TextType opt2 = INVALID_TEXT, TextType opt3 = INVALID_TEXT)
		{
			options[0] = opt1;
			options[1] = opt2;
			options[2] = opt3;
		}
	};

	void pushEntityDialogue(TextType dialogueTextType, const DialogueOptionsDTO dialogueOptions = {},
					  bool isScreenSpace = false, DialogueAlignmentType alignmentType = DIALOGUE_CENTER_ALIGNED);
	void pushCellphoneDialogue(TextType dialogueTextType, const DialogueOptionsDTO dialogueOptions = {});
	void receivePhoneCallAndPushDialogueOnAnswer(TextType dialogueTextType);

	bool isCurrentDialogue(TextType dialogueType);
	bool hasDialogueFinihsed(TextType dialogueType);
	bool hasChosenOption(TextType dialogueType);
	bool hasDialogueFinishedInterrupting(TextType dialogueType);
	bool hasAnyDialogueOngoing();

	void skipDialogue();
	void interruptCurrentDialogue();
	void destroyCurrentDialogue();

	// Array index is the decimal ASCII of the character and the value is index on font atlas.
	// For example asciiToAtlasIndex[97] = 1 means that lower case a (dec 97 asciiToAtlasIndex) is on index 1 of the font atlas.
	uint16_t _asciiToAtlasIndex[k_maxFontGlyphs]{ 0 };

	//TODO: Improve later
	Dialogue _currentDialogue;
	DialogueOption _dialogueOptions[k_maxDialogueOptions];
	float _currentTensionSpriteXSize = 0.f;
	TextType _lastDialogueType = INVALID_TEXT;
	TextType _lastOptionChosen = INVALID_TEXT;

	enum CellphoneState
	{
		CELLPHONE_NOT_VISIBLE_STATE,
		CELLPHONE_PENDING_CALL_STATE,
		CELLPHONE_TALKING
	};

	struct Cellphone
	{
		Entity* entity = nullptr;
		CellphoneState state = CELLPHONE_NOT_VISIBLE_STATE;
		TextType textToShowOnAnswer = INVALID_TEXT;
	};

	void hangupPhone();

	Cellphone _cellphone;

	bool _isTensionBarVisible = false;
	void pushTensionBar() { _isTensionBarVisible = true; }
	void popTensionBar() { _isTensionBarVisible = false; }
};