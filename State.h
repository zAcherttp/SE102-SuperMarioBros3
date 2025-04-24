class State  
{  
protected:
	Entity* m_entity;  

public:  
	State(Entity* entity) : m_entity(entity) {}  

	virtual ~State() = default;  
	virtual void Enter() = 0;  
	virtual State* HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbStates) = 0;  
	virtual void Update(float dt) = 0;  
	Entity* Exit() {  
		return m_entity;  
	}  
};
