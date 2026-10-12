#pragma once

#include <iostream>
#include <stdint.h>
#include <cstdlib>

namespace kiwi
{
  static constexpr uint32_t kDefaultSolverVarMapSize        = 16;
  static constexpr uint32_t kDefaultSolverRowMapSize        = 16;
  static constexpr uint32_t kDefaultSolverConstraintMapSize = 16;
  static constexpr uint32_t kDefaultSolverEditMapSize       = 16;
  static constexpr uint32_t kDefaultRowLength               = 16;

  typedef void*(*AllocFn)  ( size_t sz, const char* debug_name );
  typedef void (*FreeFn)   ( void* alloc_ptr );
  typedef void (*AssertFn) ( const char* msg );

  void* DefaultAllocFn ( size_t sz, const char* debug_name );
  void  DefaultFreeFn  ( void* alloc_ptr );
  void  DefaultAssertFn( const char* msg );

  struct GlobalSettings
  {
    uint32_t m_solverVarMapSize;
    uint32_t m_solverRowMapSize;
    uint32_t m_solverConstraintMapSize;
    uint32_t m_solverEditMapSize;
    uint32_t m_rowLength;

    AllocFn  m_allocFn;
    FreeFn   m_freeFn;
    AssertFn m_assertFn;

    void initDefaults()
    {
      m_solverVarMapSize        = kDefaultSolverVarMapSize;
      m_solverRowMapSize        = kDefaultSolverRowMapSize;
      m_solverConstraintMapSize = kDefaultSolverConstraintMapSize;
      m_solverEditMapSize       = kDefaultSolverEditMapSize;
      m_rowLength               = kDefaultRowLength;
      m_allocFn                 = DefaultAllocFn;
      m_freeFn                  = DefaultFreeFn;
      m_assertFn                = DefaultAssertFn;
    }
  };

  extern GlobalSettings g_globalSettings;

  inline void init( const GlobalSettings& global_settings )
  {
    g_globalSettings = global_settings;
  };
}