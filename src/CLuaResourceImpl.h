#pragma once

#include <Main.h>

class CLuaScriptRuntime;
class CLuaResourceImpl : public alt::IResource::Impl
{
public:
	class LuaFunction : public alt::IMValueFunction::Impl
	{
	public:
		LuaFunction(CLuaResourceImpl* resource, int functionRef) :
			resource(resource),
			functionRef(functionRef)
		{

		}
		~LuaFunction() {}

		alt::MValue Call(alt::MValueArgs args) const override;

	private:
		CLuaResourceImpl* resource;
		int functionRef;
	};

	typedef std::map<std::string, std::vector<int>>		EventsReferences;

	CLuaResourceImpl(CLuaScriptRuntime* runtime, alt::IResource* resource);
	~CLuaResourceImpl();

#ifdef ALT_SERVER_API
	bool MakeClient(alt::IResource::CreationInfo* info, alt::Array<alt::String> files) override;
#endif

	bool Start() override;
	bool Stop() override;

	bool OnEvent(const alt::CEvent* ev) override;
	void OnTick() override;

	void OnCreateBaseObject(alt::Ref<alt::IBaseObject> object) override;
	void OnRemoveBaseObject(alt::Ref<alt::IBaseObject> object) override;

	lua_State*	GetLuaState(void) { return this->resourceState; }
	bool		RegisterEvent(std::string eventName, int functionReference);
	bool		RemoveEvent(std::string eventName, int functionReference);
	bool		RegisterClientEvent(std::string eventName, int functionReference);
	bool		RemoveClientEvent(std::string eventName, int functionReference);
	bool		RegisterWebEvent(std::string eventName, int functionReference);
	bool		RemoveWebEvent(std::string eventName, int functionReference);
	void		TriggerResourceLocalEvent(std::string eventName, alt::MValueArgs args);
	void		IncludePath(const char* path);
	inline const std::vector<int>& GetEventReferences(std::string eventName)
	{
		return this->eventsReferences[eventName];
	}
	inline const std::vector<int>& GetClientEventReferences(std::string eventName)
	{
		return this->clientEventsReferences[eventName];
	}
	inline const std::vector<int>& GetWebEventReferences(std::string eventName)
	{
		return this->webEventsReferences[eventName];
	}
	inline bool				AddFunctionRef(const void* ptr, int functionRef)
	{
		if (this->IsFunctionRefExists(ptr))
			return false;

		this->functionReferences[ptr] = functionRef;
		return true;
	}
	inline bool				RemoveFunctionRef(const void* ptr)
	{
		if (!this->IsFunctionRefExists(ptr))
			return false;

		luaL_unref(this->resourceState, LUA_REGISTRYINDEX, this->GetFunctionRef(ptr));

		this->functionReferences.erase(ptr);
		return true;
	}
	inline bool				IsFunctionRefExists(const void* ptr)
	{
		return this->functionReferences.find(ptr) != this->functionReferences.end();
	}
	inline int				GetFunctionRef(const void* ptr)
	{
		if (!this->IsFunctionRefExists(ptr))
			return LUA_NOREF;

		return this->functionReferences[ptr];
	}
	inline const void* GetFunctionRefByID(int functionRef)
	{
		for (auto it = this->functionReferences.begin(); it != this->functionReferences.end(); ++it)
			if (it->second == functionRef)
				return it->first;

		return nullptr;
	}

	inline void				AddExport(std::string exportName, LuaFunction* func)
	{
		this->exportFunction->Set(exportName, Core->CreateMValueFunction(func));
	}
	inline bool				IsExportExists(std::string exportName)
	{
		return this->exportFunction->Get(exportName)->GetType() != alt::IMValue::Type::NONE;
	}
	inline alt::IResource* GetResource(void)
	{
		return this->resource;
	}
	inline std::string& GetWorkingPath(void)
	{
		return this->workingPath;
	}
	inline std::map<std::string, bool>& GetLoadedFiles(void)
	{
		return this->loadedFiles;
	}
#ifdef ALT_SERVER_API
	inline alt::config::Node::Dict& GetResourceConfig(void)
	{
		return this->resourceConfigDict;
	}
#endif
	inline bool AddEntity(alt::IBaseObject* baseObject)
	{
		auto entityFound = std::find(this->entities.begin(), this->entities.end(), baseObject) != this->entities.end();
		if (entityFound)
			return false;
		
		this->entities.push_back(baseObject);
		return true;
	}
	inline bool RemoveEntity(alt::IBaseObject* baseObject)
	{
		auto it = std::find(this->entities.begin(), this->entities.end(), baseObject);
		if (it == this->entities.end())
			return false;

		this->entities.erase(it);
		return true;
	}

private:
	lua_State*			resourceState = nullptr;
	CLuaScriptRuntime*	runtime;
#ifdef ALT_SERVER_API
	alt::config::Node::Dict resourceConfigDict;
#endif
	alt::IResource*		resource;
	std::string			workingPath;

	EventsReferences			eventsReferences;
	EventsReferences			clientEventsReferences;
	EventsReferences			webEventsReferences;
	std::map<const void*, int>	functionReferences;
	alt::MValueDict				exportFunction;
	std::map<std::string, bool> loadedFiles;
	std::list<alt::IBaseObject*> entities;
};