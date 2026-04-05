#pragma once

#include "lib.h"
#include "constants.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include <stdint.h>
#include "text.h"

class ECSLevel;
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

	SDL_Color _ambientColor = { 255, 255, 255, 255 };
	// Used only for debugging with imgui
	float _debugAmbientColorPicker[4] = {255, 255, 255};

	void setAmbientColor(uint8_t r, uint8_t g, uint8_t b)
	{
		_ambientColor.r = r;
		_ambientColor.g = g;
		_ambientColor.b = b;
		_ambientColor.a = 255;

#ifndef RELEASE_BUILD
		_debugAmbientColorPicker[0] = _ambientColor.r / 255.f;
		_debugAmbientColorPicker[1] = _ambientColor.g / 255.f;
		_debugAmbientColorPicker[2] = _ambientColor.b / 255.f;
#endif // !RELEASE_BUILD
	}

	SDL_Texture* loadAtlas(AtlasType type);
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
	DIALOGUE_CENTERED,
	DIALOGUE_LEFT_ALIGNED
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
		Vec2 position{ 0.f, 0.f };
		Vec2 size{ 0.f, 0.f };
		float secondsToStartShowingCharacter = 0.f;
		float opacity = 0.f;

		bool isValid()
		{
			return size.x > 0.f;
		}

		void reset()
		{
			this->atlasOffset = { 0,0 };
			this->position = { 0.f, 0.f };
			this->size = { 0.f, 0.f };
			this->secondsToStartShowingCharacter = 0.f;
			this->opacity = 0.f;
		}
	};

	enum DialogueOptionState
	{
		DIALOGUE_OPTION_IDLE_STATE,
		DIALOGUE_OPTION_HOVERED_STATE
	};

	struct DialogueOption
	{
		DialogueCharacter characters[k_maxCharactersPerDialogue];
		DialogueOptionState state = DIALOGUE_OPTION_IDLE_STATE;
		TextType dialogueType = INVALID_TEXT;

		// Used to know x,y,w,h for collisions
		SDL_FRect colliderDest;

		// Changed at runtime
		Vec2 dialogueBoxSize{ 0.f, 0.f };

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
		}

		bool isValid()
		{
			return characters[0].isValid();
		}
	};

	struct Dialogue
	{
		DialogueCharacter characters[k_maxCharactersPerDialogue];

		// Changed at runtime
		TextType dialogueType = INVALID_TEXT;
		bool hasEnded = false;
		bool isScreenSpace = false;
		float timeSinceDialogueStarted = 0.f;
		float timeSinceFinalCharacterWasDrawn = 0.f;
		Vec2 dialogueBoxSize{ 0.f, 0.f };
		Vec2 topLeftPosition{ 0.f,0.f };
		DialogueAlignmentType alignmentType = DIALOGUE_CENTERED;
		// Used to animate the dialogue box from left to right
		float dialogueBoxDynamicXSize = 0.f;

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

			this->hasEnded = false;
			this->timeSinceDialogueStarted = 0.f;
			this->timeSinceFinalCharacterWasDrawn = 0.f;
			this->dialogueBoxSize = { 0.f, 0.f };
			this->isScreenSpace = false;
			this->alignmentType = DIALOGUE_CENTERED;
			this->dialogueBoxDynamicXSize = 0.f;
			this->dialogueType = INVALID_TEXT;
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

	void pushDialogue(TextType dialogueTextType, Vec2 position, const DialogueOptionsDTO dialogueOptions = {},bool isScreenSpace = false, DialogueAlignmentType alignmentType = DIALOGUE_CENTERED);
	void pushCellphoneDialogue(TextType dialogueTextType, const DialogueOptionsDTO dialogueOptions = {});
	void receivePhoneCallAndPushDialogueOnAnswer(TextType dialogueTextType);

	bool hasDialogueFinihsed(TextType dialogueType);
	void skipDialogue();

	// Array index is the decimal ASCII of the character and the value is index on font atlas.
	// For example asciiToAtlasIndex[97] = 1 means that lower case a (dec 97 asciiToAtlasIndex) is on index 1 of the font atlas.
	uint16_t _asciiToAtlasIndex[k_maxFontGlyphs]{ 0 };

	//TODO: Improve later
	Dialogue _currentDialogue;
	DialogueOption _dialogueOptions[k_maxDialogueOptions];

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

	Cellphone _cellphone;
};