#include<windows.h>
#include<assert.h> 
#include<iostream>
#include<iomanip>
#include<list>


#ifdef  _WIN64
typedef int  long long unsigned bytes_t;
#else
typedef int  unsigned bytes_t;
#endif //  _WIN64

namespace helper
{
#undef max
#undef min
       template<typename _tp>
       int bytesof() {
              return sizeof( _tp );
       }
       int const value_one = 1;
       int const bits_of_one_byte = 8;

       template<typename _tp>
       const _tp max() {
              _tp Max = ~_tp();
              if (Max<0) {
                     Max = (~(_tp( value_one )<<(bits_of_one_byte*bytesof<_tp>()-value_one)));
              }
              return Max;
       }

}
namespace POOL_MMY
{
       void *pool_alloc( bytes_t total_for_alloc );
       void  pool_free( bytes_t total_for_alloc, void *p );
}//POOL_MMY


namespace  POOL_MMY {
       //====---
       struct link {//内存
              struct link *next;
       };
       //====---
       struct  mamager {//内存池
              bytes_t left;        //link存量
              bytes_t total;      //link总量 (存量+用量)
              bytes_t size;        //每link字节量(是粒度字节量的整数倍)
              bytes_t bytes;     //粒度字节量
              struct link *next; //内存link栈
       };
       //====---
       int const BYTES_OF_LINK_POINTER = sizeof( struct link * );
       //====---
       static int const TOTAL_POOL = 1+BYTES_OF_LINK_POINTER;
       //====---
       typedef mamager mamager_type[TOTAL_POOL];

       //六组内存管理者
       static mamager_type volatile _G_ManagerPool0 = {};
       static mamager_type volatile _G_ManagerPool1 = {};
       static mamager_type volatile _G_ManagerPool2 = {};
       static mamager_type volatile _G_ManagerPool3 = {};
       static mamager_type volatile _G_ManagerPool4 = {};
       static mamager_type volatile _G_ManagerPool5 = {};

       //====---
       enum
       {
              COUNT = 5
       };
       //====---
       static bytes_t const bytesN[1+COUNT] = {
              /*sizeof( struct link * )^1*/sizeof( struct link * ),
              /*sizeof( struct link * )^1*/sizeof( struct link * ),
              /*sizeof( struct link * )^2*/sizeof( struct link * )*sizeof( struct link * ),
              /*sizeof( struct link * )^3*/sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * ),
              /*sizeof( struct link * )^4*/sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * ),
              /*sizeof( struct link * )^5*/sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * )*sizeof( struct link * ),

       };
       //====---
       //sizeof( struct link * )^5
       static bytes_t const  POOL_MAX_SIZE = 32768; //1/32=0.03125.
       //====---
       static mamager_type volatile *_G_pool_mamager[1+COUNT] = {
       &_G_ManagerPool0,
       &_G_ManagerPool1,
       &_G_ManagerPool2,
       &_G_ManagerPool3 ,
       &_G_ManagerPool4 ,
       &_G_ManagerPool5

       };
       //====---
       void   ____free_memory_to_system___( void *p );
       void *____alloc_memory_from_system___( bytes_t allocsize, bytes_t *allocRegionSize = 0 );
       //====---
       bool static mamager_prepare_pool( /*in*/bytes_t bytes, /*out*/mamager_type volatile *&pool,/*out*/ int &idx ) {

              int idx1 = 0;
              while (bytes>bytesN[1+idx1]) {
                     idx1++;
              };
              if (idx1<COUNT) {
                     assert( idx1<COUNT );
              }
              else {
                     assert( idx1<COUNT );
              }
              pool = _G_pool_mamager[idx1];

              bytes_t const poolbytes = bytesN[idx1];

              assert( poolbytes>0 );

              int idx2 = (int)(bytes/poolbytes+(bytes%poolbytes>0));
              (*pool)[idx2].bytes = poolbytes;
              (*pool)[idx2].size = idx2*poolbytes;
              idx = idx2;


              return true;
       }
       //====---
       mamager volatile static
              *_G_hashPOOL[1+POOL_MAX_SIZE] = { };
       //====---
       mamager volatile static
              *mamager_get( bytes_t total_for_alloc )
       {
              assert( _G_hashPOOL[total_for_alloc]==0 );
              {

                     mamager_type volatile *pool = 0;
                     int idx = 0;
                     mamager_prepare_pool( total_for_alloc, pool, idx );
                     if (idx>=TOTAL_POOL) {
                            throw std::exception( "get_pool error" );
                            return 0;
                     }
                     _G_hashPOOL[total_for_alloc] = &(*pool)[idx];
              }
              return _G_hashPOOL[total_for_alloc];
       }
       //====---
       void static
              mamager_init_pool( mamager volatile *pool ) {
              bytes_t const sizeofmem = pool->size;
              //分内存,认内存( 分出一些Windows操作系统的 本进程的 虚拟内存)
              //李夫之一见三C。
              bytes_t   maxofmem = sizeofmem;
              char *p = (char *)____alloc_memory_from_system___( sizeofmem, &maxofmem );

              bytes_t n = maxofmem/sizeofmem;
              if ((*pool).total>::helper::max<bytes_t>()-n) {
                     throw std::exception( "init error" );
                     return;
              }

              if (p==0) {
                     throw std::exception( "init error" );
                     return;
              }

              // ::memset( p, 0, sizeofmem*n );
              assert( (*pool).size==sizeofmem );
              (*pool).total += n;
              (*pool).left += n;

              //办内存链( 这里是一个单链表)
              ((link *)p)->next = (*pool).next;
              while (--n>0) {
                     char *pp = (p+sizeofmem);
                     ((link *)pp)->next = ((link *)p);
                     p = pp;
              };
              (*pool).next = ((link *)p);
       }
       //====---
       void
              pool_free( bytes_t total_for_alloc, void *p ) {
              if (p==0||total_for_alloc==0)
                     return;
              if (total_for_alloc>POOL_MAX_SIZE) {
                     POOL_MMY::____free_memory_to_system___( p );
                     return;
              }
              mamager volatile *pool = _G_hashPOOL[total_for_alloc];
              if (pool==0) {
                     throw std::exception( "pool_free error" );
                     return;
              }
              // ::memset( p, 0, total_for_alloc );
              ((link *)p)->next = (*pool).next;
              (*pool).next = ((link *)p);
              (*pool).left = (*pool).left+(+1);

       }
       //====---
       int long long unsigned static
              _G_alloc_total_size = 0;

       void static *
              ____alloc_memory_from_system___( bytes_t  const allocsize, bytes_t *allocRegionSize ) {
              void *p = 0;
              int loops = 12;
              do {
                     //分内存(char *) ,认内存,
                     p = ::VirtualAlloc( 0, allocsize, MEM_COMMIT|MEM_RESERVE|MEM_TOP_DOWN, PAGE_READWRITE );
                     if (p==0) {
                            std::cout<<"VirtualAlloc memory error code:"<<::GetLastError()<<"\r\n";
                            ::Sleep( 1000 );
                     }
              } while (p==0&&--loops>0);

              MEMORY_BASIC_INFORMATION  info = {};
              ::VirtualQuery( p, &info, sizeof( info ) );

              if (allocRegionSize) {
                     *allocRegionSize = info.RegionSize;
                     _G_alloc_total_size += info.RegionSize;
              }
              else {
                     _G_alloc_total_size += allocsize;
              }

              return p;
       }
       //====---
       void   static
              ____free_memory_to_system___( void *p ) {

              MEMORY_BASIC_INFORMATION  info = {};
              ::VirtualQuery( p, &info, sizeof( info ) );

              _G_alloc_total_size = _G_alloc_total_size-info.RegionSize;

              ::VirtualFree( p, 0, MEM_RELEASE );

       }
       //====---
       void
              *pool_alloc( bytes_t total_for_alloc ) {

              if (total_for_alloc==0) {
                     return 0;
              }
              if (total_for_alloc>POOL_MAX_SIZE) {
                     return	POOL_MMY::____alloc_memory_from_system___( total_for_alloc );
              }

              mamager volatile *pool = _G_hashPOOL[total_for_alloc];
              if (0==pool) {
                     pool = mamager_get( total_for_alloc );
                     if (pool==0) {
                            throw std::exception( "pool_alloc error1" );
                            return 0;
                     }
              };
              if (0==(*pool).next) {
                     mamager_init_pool( pool );
                     if (0==(*pool).next) {
                            throw std::exception( "pool_alloc erro2" );
                            return 0;
                     }
              };
              void *pMem = (*pool).next;
              {
                     (*pool).next = (*pool).next->next;
                     (*pool).left = (*pool).left+-1;
              }
              ((::POOL_MMY::link *)pMem)->next = 0;

              return pMem;

       }
       //====---


}//POOL_MMY

//====---
namespace Ligongbinde {

       //---------------------------
       template<typename __tp, typename baseClass>
       class Alloc : public  baseClass
       {
       protected:
              Alloc() {}
              virtual ~Alloc() {  }
              void *operator new(bytes_t total_bytes) {
                     int size = sizeof __tp;
                     assert( sizeof __tp==total_bytes );
                     return POOL_MMY::pool_alloc( total_bytes );
              }
              void operator delete(void *p) {
                     POOL_MMY::pool_free( sizeof __tp, p );
              }
       public:

              static __tp *create() { return new __tp(); }

              template<typename __tp2>
              static __tp *create( __tp2 const &v ) { return new __tp( v ); }

              template<typename __tp2, typename __tp3>
              static __tp *create( __tp2 const &v, __tp3 const &vv ) { return new __tp( v, vv ); }

              template<typename __tp2, typename __tp3, typename __tp1>
              static __tp *create( __tp2 const &v, __tp3 const &vv, __tp1 const &vvv ) { return new __tp( v, vv, vvv ); }

              virtual void destroy() { delete this; }
       };
       //---------------------------
       template<class _Ty>struct allocator_base { typedef _Ty value_type; };
       template<class _Ty>struct allocator_base<const _Ty> { typedef _Ty value_type; };

       template<class _Ty>class Allocator : public allocator_base<_Ty>
       {
       public:
              typedef allocator_base<_Ty> baseClass;
              typedef typename baseClass::value_type value_type;

              typedef value_type *pointer;
              typedef value_type &reference;
              typedef const value_type *const_pointer;
              typedef const value_type &const_reference;

              typedef  bytes_t size_type;
              typedef  int  difference_type;

              template<class _Other>struct rebind { typedef Allocator<_Other> other; };

              pointer address( reference _Val ) const { return ((pointer)&(char &)_Val); }

              const_pointer address( const_reference _Val ) const { return ((const_pointer)&(char &)_Val); }

              Allocator() {	 }
              Allocator( const Allocator<_Ty> & ) {	}
              template<class _Other>Allocator( const Allocator<_Other> & ) { }
              template<class _Other>Allocator<_Ty> &operator=( const Allocator<_Other> & ) { return (*this); }

              void deallocate( pointer _Ptr, size_type )
              {
                     POOL_MMY::pool_free( sizeof value_type, _Ptr );
              }
              pointer allocate( size_type _Count )
              {
                     assert( _Count==1 );
                     return (pointer)POOL_MMY::pool_alloc( sizeof value_type*_Count );
              }

              pointer allocate( size_type _Count, const void * )
              {
                     return (allocate( _Count ));
              }

              void construct( pointer _Ptr, const _Ty &_Val ) {
                     ::new (_Ptr) _Ty( _Val );
              }

              void construct( pointer _Ptr, _Ty &&_Val )
              {
                     ::new ((void *)_Ptr) value_type( _Val );
              }

              template<class _Other>void construct( pointer _Ptr, _Other &&_Val )
              {
                     ::new ((void *)_Ptr) value_type( _Val );
              }

              void destroy( pointer _Ptr )
              {
                     _Ptr->~_Ty();
              }

              size_type max_size() const
              {	// estimate maximum array size
                     //size_type _Count = (POOL_MMY::maxV<size_type>())/sizeof( _Ty );
                    // return (0<_Count ? _Count : 1);
                     static  const size_type _Count = (helper::max<int>())/sizeof( _Ty );
                     return _Count;


              }
       };

};



int t1() {

       struct node_link {
              node_link *up, *down;
              int data;
       };
       int const N = 10000;
       node_link **ary = (node_link **)::POOL_MMY::pool_alloc( sizeof( node_link * )*N );

       for (int i = 0; i<N; i++) {

              ary[i] = (node_link *)::POOL_MMY::pool_alloc( sizeof( node_link ) );
       }
       for (int i = 0; i<N; i++) {

              ::POOL_MMY::pool_free( sizeof( node_link ), ary[i] );
       }

       ::POOL_MMY::pool_free( sizeof( int * )*N, ary );


       return 0;
}

struct node_link {
       node_link *up, *down;
       int data;
};

void t2() {


       {
              ::std::list<node_link, ::Ligongbinde::Allocator<node_link>> list;

              node_link n = { 0 };

              list.resize( 1000 );
       }

       {
              ::std::list<long long int, ::Ligongbinde::Allocator<long long int>> list;

              list.resize( 10000 );
       }
       for (int i = 0; i<POOL_MMY::POOL_MAX_SIZE; i++) {

              if (POOL_MMY::_G_hashPOOL[i]) {
                     std::cout<<typeid(POOL_MMY::_G_hashPOOL[i]).name(); std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"left: "<<POOL_MMY::_G_hashPOOL[i]->left; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"total: "<<POOL_MMY::_G_hashPOOL[i]->total; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"size: "<<POOL_MMY::_G_hashPOOL[i]->size; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"bytes: "<<POOL_MMY::_G_hashPOOL[i]->bytes; std::cout<<"\r\n";

              }
       }

}


int main() {
       t1();

       for (int i = 0; i<POOL_MMY::POOL_MAX_SIZE; i++) {

              if (POOL_MMY::_G_hashPOOL[i]) {
                     std::cout<<typeid(POOL_MMY::_G_hashPOOL[i]).name(); std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"left: "<<POOL_MMY::_G_hashPOOL[i]->left; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"total: "<<POOL_MMY::_G_hashPOOL[i]->total; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"size: "<<POOL_MMY::_G_hashPOOL[i]->size; std::cout<<"\r\n";
                     std::cout<<std::right<<std::setw( 7 )<<"bytes: "<<POOL_MMY::_G_hashPOOL[i]->bytes; std::cout<<"\r\n";

              }
       }
}