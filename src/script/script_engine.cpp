#include "script/script_engine.h"
#include "script/layers.h"
#include "script/parameters.h"

#include <FastNoise.h>

#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <memory>

ScriptEngine::ScriptEngine(const std::string& filename) :
    script_(filename)
{
}

ScriptEngine::~ScriptEngine()
{
}

struct A
{
    void f(int);
    void f(int, int);
};

GeneratorScript::Ptr ScriptEngine::createGenerator()
{
    using namespace luabind;

    // create a lua instance
    lua_State* L = lua_open();

    // open the lua instance and import libs
    open(L);
    luaL_openlibs(L);

    // enable JIT
    luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
    //luaJIT_setmode(L, 0, LUAJIT_MODE_ALLFUNC | LUAJIT_MODE_ON);

    // bind c++ classes to lua instance
    module(L)[
        class_<Color>("Color")
            .def(constructor<float>())
            .def_readwrite("r", &Color::r)
            .def_readwrite("g", &Color::g)
            .def_readwrite("b", &Color::b)
            .def_readwrite("a", &Color::a),

        class_<FastNoise>("Noise")
            .def("sample",    (float(FastNoise::*)(float, float) const)&FastNoise::GetNoise)
            .def("sample",    (float(FastNoise::*)(float, float, float) const)&FastNoise::GetNoise)
            .def("setLookup", &FastNoise::SetCellularNoiseLookup),

        class_<Layers>("Layers")
            .def("set",    &Layers::set)
            .def("startX", &Layers::startX)
            .def("startY", &Layers::startY)
            .def("endX",   &Layers::endX)
            .def("endY",   &Layers::endY)
            .def("width",  &Layers::getWidth)
            .def("height", &Layers::getHeight),

        class_<Parameters>("Parameters")
            .def("getNoise", &Parameters::getNoise)
            .def("getColor", &Parameters::getColor)
            .def("getFloat", &Parameters::getFloat)
    ];

    // load the file
    luaL_dofile(L, script_.c_str());

    // grab file metadata...

    return GeneratorScript::Ptr(new GeneratorScript(L));
}
