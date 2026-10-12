#include "settings.h"

kiwi::GlobalSettings kiwi::g_globalSettings;

namespace kiwi
{
  void* DefaultAllocFn( size_t sz, const char* debug_name )
  {
    (void)(debug_name);
    return malloc( sz );
  }

  void DefaultFreeFn( void* alloc_ptr )
  {
    free( alloc_ptr );
  }

  void DefaultAssertFn( const char* msg )
  {
    std::cout << msg;
    // crash
    uint8_t var = *(uint8_t*)0;
  }
}