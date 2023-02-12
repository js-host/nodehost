#pragma once

#ifndef NODEHOST_CONFIG_H
#define NODEHOST_CONFIG_H

#if defined(_MSC_VER)
#define NODEHOST_EXPORT __declspec(dllexport)
#define NODEHOST_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define NODEHOST_EXPORT __attribute__((visibility("default")))
#define NODEHOST_IMPORT
#else
#define NODEHOST_EXPORT
#define NODEHOST_IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef NODEHOST_COMPILING
#   define NODEHOST_API NODEHOST_EXPORT
#else
#   define NODEHOST_API NODEHOST_IMPORT
#endif

#endif
