# Install script for directory: E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.2-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/lib/Debug/assimp-vc145-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/lib/Release/assimp-vc145-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/lib/MinSizeRel/assimp-vc145-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/lib/RelWithDebInfo/assimp-vc145-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.2" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/Debug/assimp-vc145-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/Release/assimp-vc145-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/MinSizeRel/assimp-vc145-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/RelWithDebInfo/assimp-vc145-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/anim.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/aabb.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ai_assert.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/camera.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/color4.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/color4.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/code/../include/assimp/config.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ColladaMetaData.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/commonMetaData.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/defs.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/cfileio.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/light.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/material.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/material.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/matrix3x3.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/matrix3x3.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/matrix4x4.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/matrix4x4.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/mesh.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ObjMaterial.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/pbrmaterial.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/GltfMaterial.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/postprocess.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/quaternion.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/quaternion.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/scene.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/metadata.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/texture.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/types.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/vector2.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/vector2.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/vector3.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/vector3.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/version.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/cimport.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/AssertHandler.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/importerdesc.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Importer.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/DefaultLogger.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ProgressHandler.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/IOStream.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/IOSystem.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Logger.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/LogStream.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/NullLogger.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/cexport.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Exporter.hpp"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/DefaultIOStream.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/DefaultIOSystem.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ZipArchiveIOSystem.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SceneCombiner.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/fast_atof.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/qnan.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/BaseImporter.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Hash.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/MemoryIOWrapper.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ParsingUtils.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/StreamReader.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/StreamWriter.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/StringComparison.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/StringUtils.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SGSpatialSort.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/GenericProperty.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SpatialSort.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SkeletonMeshBuilder.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SmallVector.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SmoothingGroups.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/SmoothingGroups.inl"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/StandardShapes.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/RemoveComments.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Subdivision.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Vertex.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/LineSplitter.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/TinyFormatter.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Profiler.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/LogAux.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Bitmap.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/XMLTools.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/IOStreamBuffer.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/CreateAnimMesh.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/XmlParser.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/BlobIOSystem.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/MathFunctions.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Exceptional.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/ByteSwapper.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Compiler/pushpack1.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Compiler/poppack1.h"
    "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/Debug/assimp-vc145-mtd.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/Release/assimp-vc145-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/MinSizeRel/assimp-vc145-mt.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/bin/RelWithDebInfo/assimp-vc145-mt.pdb")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "E:/project/Quavle-Engine/Quavle-Engine/vendor/assimp-6.0.2/build/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
