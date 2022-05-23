#include <bits/stdc++.h>
#include "BMP.h"
using namespace std;

//From degree to radian
double degtorad(double deg)
{
    return (deg*M_PI/180);
}

//From radian to degree
double radtodeg(double rad)
{
    return (rad*180/M_PI);
}

//Reverse pixel data of bitmap
vector<uint8_t> vrev(vector<uint8_t> v,uint32_t c,uint32_t w,uint32_t h)
{
    vector<uint8_t> rv;
    rv.resize(c*w*h);
    for (int i=0;i<h;i++)
    {
        for (int j=0;j<w;j++)
        {
            rv[c*(i*w+j)+2]=v[c*((h-(i+1))*w+j)];
            rv[c*(i*w+j)+1]=v[c*((h-(i+1))*w+j)+1];
            rv[c*(i*w+j)]=v[c*((h-(i+1))*w+j)+2];
        }
    }
    return rv;
}

//Point (x,y)
struct Point
{
    double x,y;
    Point()
    {

    }
    Point(double _x,double _y)
    {
        x=_x;
        y=_y;
    }
};

//Rotate point around another point
Point rotaround(Point cen,double alpha,Point p)
{
    double s=sin(alpha);
    double c=cos(alpha);
    p.x-=cen.x;
    p.x-=cen.y;
    double xnew=p.x*c-p.y*s;
    double ynew=p.x*s+p.y*c;
    p.x=xnew+cen.x;
    p.y=ynew+cen.y;
    return p;
}

//Color as RGB/RGBA
struct Color
{
    uint8_t r,g,b,a;
    Color()
    {
        r=0;
        g=0;
        b=0;
        a=false;
    }
    Color(uint8_t _r,uint8_t _g,uint8_t _b,bool _a=false)
    {
        r=_r;
        g=_g;
        b=_b;
        a=_a;
    }
};

//Main program
int main()
{
    //Open bitmap (Call it BMP1)
    BMP bmp1("flower.bmp");

    //Width and height of BMP1
    int w1=bmp1.bmp_info_header.width,h1=bmp1.bmp_info_header.height;

    //Center point of BMP1
    Point center((double)w1/2,(double)h1/2);

    //Rotation degree-alpha,we'll convert to and use as radian
    double alpha;
    cin>>alpha;
    alpha=degtorad(alpha);

    //Size of rotated image
    Point _bef[4],_aft[4];
    _bef[0]=Point(0,0);
    _bef[1]=Point(w1,0);
    _bef[2]=Point(0,h1);
    _bef[3]=Point(w1,h1);
    for (int i=0;i<4;i++) _aft[i]=rotaround(center,alpha,_bef[i]);

    int w2=0,h2=0;
    double xmin=INT_MAX,ymin=INT_MAX;
    for (int i=0;i<4;i++)
    {
        double dw,dh;
        for (int j=0;j<4;j++)
        {
            dw=abs(_aft[i].x-_aft[j].x);
            dh=abs(_aft[i].y-_aft[j].y);
            if (dw>(int)dw) dw++;
            if (dh>(int)dh) dh++;
            w2=max((double)w2,dw);
            h2=max((double)h2,dh);
        }
    }

    //Get minimum x and minimum y
    for (int i=0;i<4;i++)
    {
        for (int j=0;j<4;j++)
        {
            xmin=min(xmin,_aft[i].x);
            ymin=min(ymin,_aft[i].y);
        }
    }

    //Create bitmap (Call it BMP2) with size w2 width and h2 height
    BMP bmp2(w2,h2,false);

    //Get data from BMP1
    pair<vector<uint8_t>,uint32_t> p=bmp1.getregion(0,0,w1,h1);

    //We'll reverse the pixel data of BMP1 for work on it
    vector<uint8_t> rv1=vrev(p.first,p.second,w1,h1),rv2;

    //Declare array for store pixel data of BMP2
    rv2.resize(3*w2*h2);

    //Information about emptiness of pixel data array of BMP2
    vector<bool> rv_empty(w2*h2);

    //Set emptiness information of all pixels true
    for (int i=0;i<h2;i++)
    {
        for (int j=0;j<w2;j++)
        {
            rv2[3*(i*w2+j)]=0;
            rv2[3*(i*w2+j)+1]=0;
            rv2[3*(i*w2+j)+2]=0;
            rv_empty[i*w2+j]=true;
        }
    }

    //Copy each pixel of BMP2 to BMP1 as rotated around center
    for (int i=0;i<h1;i++)
    {
        for (int j=0;j<w1;j++)
        {
            Point pt=rotaround(center,alpha,Point(j,i));
            pt.x-=xmin;
            pt.y-=ymin;
            int pos=(int)pt.y*w2+(int)pt.x;
            if (pos<w2*h2)
            {
                rv2[3*pos]=rv1[p.second*(i*w1+j)];
                rv2[3*pos+1]=rv1[p.second*(i*w1+j)+1];
                rv2[3*pos+2]=rv1[p.second*(i*w1+j)+2];
                rv_empty[pos]=false;
            }
        }
    }

    //Filling the gaps in the image caused by the completion of the fraction
    for (int i=1;i<h2-1;i++)
    {
        for (int j=1;j<w2-1;j++)
        {
            if (rv2[3*(i*w2+j)]==0 && rv2[3*(i*w2+j)+1]==0 && rv2[3*(i*w2+j)+2]==0 && rv_empty[i*w2+j])
            {
                //Four neighbour principle
                Color c[4];
                c[0]=Color(rv2[3*((i-1)*w2+j)],rv2[3*((i-1)*w2+j)+1],rv2[3*((i-1)*w2+j)+2]);
                c[1]=Color(rv2[3*(i*w2+j+1)],rv2[3*(i*w2+j+1)+1],rv2[3*(i*w2+j+1)+2]);
                c[2]=Color(rv2[3*((i+1)*w2+j)],rv2[3*((i+1)*w2+j)+1],rv2[3*((i+1)*w2+j)+2]);
                c[3]=Color(rv2[3*(i*w2+j-1)],rv2[3*(i*w2+j-1)+1],rv2[3*(i*w2+j-1)+2]);

                //Set RGB of empty pixel equal to numerical average of neigbours
                int r=0,g=0,b=0;
                for (int k=0;k<4;k++)
                {
                    r+=c[k].r;
                    g+=c[k].g;
                    b+=c[k].b;
                }
                r/=4;
                g/=4;
                b/=4;
                rv2[3*(i*w2+j)]=r;
                rv2[3*(i*w2+j)+1]=g;
                rv2[3*(i*w2+j)+2]=b;
            }
        }
    }

    //Reverse pixel data for write to BMP2
    rv2=vrev(rv2,3,w2,h2);

    //Write pixels to BMP2
    for (int i=0;i<h2;i++)
        for (int j=0;j<w2;j++)
            bmp2.set_pixel(j,i,rv2[3*(i*w2+j)],rv2[3*(i*w2+j)+1],rv2[3*(i*w2+j)+2],0);

    //Save BMP2
    bmp2.write("image.bmp");
    return 0;
}

