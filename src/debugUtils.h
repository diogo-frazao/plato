#pragma once

#include "level.h"
#include "core/lib.h"
#include "imgui.h"
#include "SDL3/SDL_clipboard.h"

inline Entity* s_entitiesThatChangedViaInspector[200];
inline uint8_t s_lastEntityThatChangedIndex = 0;
inline constexpr int k_bufferSize = 1024 * 1024;
inline char s_printComponentsToClipboardBuffer[k_bufferSize];
inline size_t s_currentWriteByte = 0;

inline void registerEntityChangeAtRuntime(Entity* entity)
{
    for (Entity* entityAtIndex : s_entitiesThatChangedViaInspector)
    {
        if (entityAtIndex && entityAtIndex->id == entity->id)
        {
            return;
        }
    }

    s_entitiesThatChangedViaInspector[s_lastEntityThatChangedIndex] = entity;
    s_lastEntityThatChangedIndex++;
}

inline void copyAllChangesToClipboard()
{
    for (Entity* entity : s_entitiesThatChangedViaInspector)
    {
        if (!entity)
        {
            continue;
        }

        getComponentFromEntity<TransformComponent>(*entity)->writeComponentDataToBuffer(entity->debugName, s_printComponentsToClipboardBuffer, k_bufferSize, &s_currentWriteByte);

        if (entityHasComponent<SpriteComponent>(*entity))
        {
            getComponentFromEntity<SpriteComponent>(*entity)->writeComponentDataToBuffer(entity->debugName, s_printComponentsToClipboardBuffer, k_bufferSize, &s_currentWriteByte);
        }
    }

    if (!SDL_SetClipboardText(s_printComponentsToClipboardBuffer))
    {
        D_LOG(ERROR, "%s", SDL_GetError());
    }

    memset(s_printComponentsToClipboardBuffer, 0, k_bufferSize - s_currentWriteByte);
    s_currentWriteByte = 0;

    D_LOG(LOG, "Copied all changes to clipboard");
}

template<typename T>
bool createInspectorComponentSeparator(Entity* entity)
{
	if (!entityHasComponent<T>(*entity))
	{
		return false;
	}

	ImGui::Separator();
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::Text(typeid(T).name());
	ImGui::PopStyleColor();

	return true;
}

template<typename T>
bool startInspectorComponentSection(Entity* entity)
{
    if (!createInspectorComponentSeparator<T>(entity))
    {
        return false;
    }

    if (!ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
    {
        return false;
    }

    ImGui::PushID(entity->id);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);
    return true;
}

inline void endInspectorComponentSection()
{
    ImGui::PopID();
    ImGui::EndTable();
}

inline void inspectFloatProperty(char* name, float* variable, Entity* entity)
{
    ImGui::TableNextRow();
    ImGui::PushID(name);
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::SliderFloat("##Editor", variable, *variable - 1.f, *variable + 1.f))
    {
        registerEntityChangeAtRuntime(entity);
    }

    ImGui::PopID();
}

inline void inspectColorProperty(char* name, SDL_Color* color, Entity* entity)
{
    static float colorArray[4];
    colorArray[0] = (float)color->r / 255.f;
    colorArray[1] = (float)color->g / 255.f;
    colorArray[2] = (float)color->b / 255.f;
    colorArray[3] = (float)color->a / 255.f;

    ImGui::TableNextRow();
    ImGui::PushID(name);
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::ColorEdit4(name, colorArray))
    {
        color->r = colorArray[0] * 255;
        color->g = colorArray[1] * 255;
        color->b = colorArray[2] * 255;
        color->a = colorArray[3] * 255;
        registerEntityChangeAtRuntime(entity);
    }

    ImGui::PopID();
}

template<typename Enum>
void inspectEnumProperty(char* name, Enum* currentEnumType, const char* allEnumTypesAsString, Entity* entity)
{
    int currentEnumTypeAsInt = (int)*currentEnumType;

    ImGui::TableNextRow();
    ImGui::PushID(name);
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::Combo(name, &currentEnumTypeAsInt, allEnumTypesAsString))
    {
        *currentEnumType = (Enum)currentEnumTypeAsInt;
        registerEntityChangeAtRuntime(entity);
    }

    ImGui::PopID();
}

inline void inspectSpriteProperty(char* name, SpriteComponent* s, Entity* entity)
{
    int currenSpriteAsInt = (int)s->sprite;

    ImGui::TableNextRow();
    ImGui::PushID(name);
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::Combo(name, &currenSpriteAsInt, s_allSpritesAsString))
    {
        SpriteType newSpriteType = (SpriteType)currenSpriteAsInt;
        s->setSpriteData(newSpriteType);
        registerEntityChangeAtRuntime(entity);
    }

    ImGui::PopID();
}