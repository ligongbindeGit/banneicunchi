
template<typename __tp>
struct point {
       typedef __tp value_type;
       value_type x, y;
       point( value_type xx, value_type  yy ) :x( xx ), y( yy ) {}
};

typedef point<double> pointB; //8*2

struct line {
       pointB a;
       pointB b;
       line( pointB::value_type x1, pointB::value_type y1, pointB::value_type x2, pointB::value_type y2 )
              :a( x1, y1 ), b( x2, y2 )
       {

       }
};//8*2*2

struct circle {
       pointB a;
       pointB::value_type r;
       circle( pointB::value_type  x, pointB::value_type y, pointB::value_type _r )
              :a( x, y ), r( _r )
       {}
};//8*2+8  //24

class Disk {
};

class IAlloc {
protected:
       IAlloc() {}
       virtual ~IAlloc() {}
public:
       virtual void destroy() = 0;
};

class IDraw : public IAlloc { //4
protected:
       IDraw() {}
       virtual ~IDraw() { }
public:
       virtual void Draw() = 0;
       virtual void Save( Disk & ) = 0;
       virtual void Read( Disk & ) = 0;
};


template<typename __tp, typename baseClass>
class Alloc; 

class DrawLine : public Alloc<DrawLine, IDraw> {//4
       friend Alloc<DrawLine, IDraw>;
       line data;
public: virtual void Draw() {};
public:virtual void Save( Disk & ) {};
public:virtual void Read( Disk & ) {};

protected:
       DrawLine( line const &d ) : data( d ) {};
};

class DrawCircle : public    Alloc<DrawCircle, IDraw> {

       friend Alloc<DrawCircle, IDraw>;

       circle data;
public: virtual void Draw() {};
public: virtual void Save( Disk & ) {};
public:virtual void Read( Disk & ) {};

protected:
       DrawCircle( circle const &d ) : data( d )
       {};
       virtual ~DrawCircle()
       {}

};

class Document {
public:

       std::list<::IDraw *, Ligongbinde::Allocator<::IDraw *>> listDraw;
};


void test1()
{
       int long unsigned times1, times2, times3;

       srn::showcursor( false );
       while (true) {
              srn::gotoxy( 0, 0 );
              std::cout<<"内存: "<<::POOL_MMY::_G_alloc_total_size;
              {
                     std::list<bytes_t, ::Ligongbinde::Allocator<bytes_t>> list1;
                     std::list<void *, ::Ligongbinde::Allocator<void *>> list2;
                     srn::gotoxy( 0, 1 );
                     std::cout<<"分配内存 用时: ";
                     times1 = ::GetTickCount();

                     for (int m = 4; m<100000; m += 17) {
                            for (int mem = rand()%8192, i = 1; i<100; i += 1) {
                                   void *p = POOL_MMY::pool_alloc( mem );
                                   list1.push_back( mem );
                                   list2.push_back( p );
                            }
                     }





                     {
                            bytes_t  mem = ::GetLargePageMinimum();
                            void *p = POOL_MMY::pool_alloc( mem );
                            list1.push_back( mem );
                            list2.push_back( p );
                     }

                     times2 = ::GetTickCount();
                     std::cout<<""<<times2-times1<<"\r\n";

                     srn::gotoxy( 0, 3 );
                     std::cout<<"释放内存 用时: ";
                     times2 = ::GetTickCount();

                     auto i = list1.begin();
                     auto ii = list2.begin();
                     for (; i!=list1.end(); i++, ii++) {

                            POOL_MMY::pool_free( (*i), (*ii) );
                     }
                     list1.clear();
                     list2.clear();

                     auto i1 = list1.begin();

              }
              times3 = ::GetTickCount();
              std::cout<<""<<times3-times2<<"\r\n";
       }



}


int test2() {



       do {
              int long unsigned times1, times2, times3;
              {
                     ::Document d;

                     int n = 20359540;//d.listDraw.max_size()/2-1;

                     srn::gotoxy( 0, 0 );

                     std::cout<<"\r\n";
                     std::cout<<"分配内存 ";
                     times1 = ::GetTickCount();

                     while (--n>0) {

                            try {
                                   d.listDraw.push_back( ::DrawCircle::create( ::circle( 0, 0, 100 ) ) );
                                   d.listDraw.push_back( ::DrawLine::create( ::line( 0, 0, 100, 100 ) ) );
                            }
                            catch (std::exception &e) {
                                   std::cout<<e.what();
                                   break;
                            }
                     }

                     times2 = ::GetTickCount();
                     std::cout<<""<<times2-times1<<"\r\n";

                     for (auto i = d.listDraw.begin(); i!=d.listDraw.end(); i++) {
                            (*i)->Draw();
                     }
                     srn::gotoxy( 0, 3 );
                     std::cout<<"释放内存 ";
                     times2 = ::GetTickCount();
                     for (auto i = d.listDraw.begin(); i!=d.listDraw.end(); i++) {
                            (*i)->destroy();
                     }
                     d.listDraw.clear();


              }

              times3 = ::GetTickCount();
              std::cout<<""<<times3-times2<<"\r\n";


       } while (1+1==2);

       exit( 0 );
}
