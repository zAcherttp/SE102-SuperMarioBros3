#pragma once
#include <vector>
#include <memory>
#include "SpriteSheet.h"
#include "pch.h"

using namespace DirectX::SimpleMath; 

class Effect;
enum class EffectType;

class EffectManager {
private:
    // Singleton instance
    static EffectManager* s_instance;
    
    // Collection of active effects
    std::vector<std::unique_ptr<Effect>> m_effects;
    
    // Sprite sheet reference for creating effects
    SpriteSheet* m_spriteSheet;

public:
    EffectManager(SpriteSheet* spriteSheet = nullptr);
    ~EffectManager();
    
    // Get singleton instance
    static EffectManager* GetInstance() { return s_instance; }
    
    // Set sprite sheet
    void SetSpriteSheet(SpriteSheet* spriteSheet) { m_spriteSheet = spriteSheet; }
    
    // Create different types of effects
    Effect* CreateBonkEffect(const Vector2& position);
    Effect* CreatePointEffect(const Vector2& position, int points);
    Effect* CreateCoinEffect(const Vector2& position);
    Effect* CreateBrickEffect(const Vector2& position);
    Effect* CreateSmokeEffect(const Vector2& position);
    
    // General create effect method
    Effect* CreateEffect(const Vector2& position, Vector2 size, EffectType type);
    
    // Update all effects
    void Update(float dt);
    
    // Render all effects
    void Render(DirectX::SpriteBatch* spriteBatch);
    
    // Clear all effects
    void ClearEffects();
    
    // Get count of active effects
    size_t GetEffectCount() const { return m_effects.size(); }
};