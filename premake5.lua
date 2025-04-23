workspace "Workspace"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "QSD"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "QSD"