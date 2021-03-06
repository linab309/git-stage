#include "sys.h"
#include "string.h"
#include "math.h"
#include "Gongshi.h"
#include "oled.h"
#include "string.h"
#include "gps.h"
#include "usart.h"
#include "menutal.h"

#define EARTH_RADIUS  6371.004

#define PI 3.14159

float rad(float d)

{

   return d * PI / 180.0;

}




float getDistanceVer1(float lat1, float lng1, float lat2, float lng2)
{

   float radLat1 = rad(lat1);

   float radLat2 = rad(lat2);

   float radLng1 = rad(lng1);

   float radLng2 = rad(lng2);

   float s = acos(sin(radLat1)*sin(radLat2)+cos(radLat1)*cos(radLat2)*cos(radLng1-radLng2));

   s = s * EARTH_RADIUS;

   return s;

}




float getDistanceVer2(float lat1, char latitude1_flag,float lng1, char longitude1_flag, float lat2,char latitude2_flag, float lng2,char longitude2_flag)

{

    float radLat1 ,radLat2,a,b,s;

    if(longitude1_flag=='W')
    {
        lng1 = 360-lng1;
    }
    if(latitude1_flag=='S')
    {
        lat1 = -1*lat1;
    }
    if(latitude2_flag=='S')
    {
        lat2 = -1*lat2;
    }
    if(longitude2_flag=='W')
    {
        lng2 = 360-lng2;
    }

   radLat1= rad(lat1);

   radLat2 = rad(lat2);

   a = radLat1 - radLat2;

   b = rad(lng1) - rad(lng2);

   s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));

   s = s * EARTH_RADIUS;

   return s;

}





#if 1

#define Rc  6378137  // 赤道半径

#define Rj   6356725  // 极半径


 //! 计算点A 和 点B的经纬度，求他们的距离和点B相对于点A的方位
/*!
  * /param A A点经纬度
  * /param B B点经纬度
  * /param angle B相对于A的方位, 不需要返回该值，则将其设为空
  * /return A点B点的距离
  */
float get_angle(float latitude1, char latitude1_flag,float longitude1, char longitude1_flag,
                 float latitude2, char latitude2_flag, float longitude2, char longitude2_flag)
{

    float A_Ec, A_Ed,angle;
    float A_m_RadLo, A_m_RadLa,B_m_RadLo,B_m_RadLa;
    float dx,dy,dLo,dLa;

    A_m_RadLo  = longitude1 * PI / 180.;
    A_m_RadLa  = latitude1 * PI / 180.;

    A_Ec = Rj + ((Rc - Rj) * (90- latitude1) / 90);
    A_Ed = A_Ec * cos(A_m_RadLa);


    B_m_RadLo  = longitude2 * PI / 180.;
    B_m_RadLa  = latitude2 * PI / 180;
   // B_Ec = Rj + (Rc - Rj) * (90- latitude2) / 90;
   // B_Ed = B_Ec * cos(B_m_RadLa);

    dx = (B_m_RadLo - A_m_RadLo) * A_Ed;
    dy = (B_m_RadLa - A_m_RadLa) * A_Ec;
//   out = sqrt(dx * dx + dy * dy);

    if( &angle != NULL)
    {
        angle = atan(fabs(dx/dy))*180/PI;
        // 判断象限

        dLo = longitude2 - longitude1;
        dLa = latitude2 - latitude1;


        if((latitude1_flag=='S')&&(longitude1_flag=='W'))
        {

#if 1
                        
            if(dLo <= 0&&dLa >= 0){  
                angle = (90-angle)+90;  
            }  
            else if(dLo > 0&&dLa >= 0){  
                angle = angle+180.;  
            }else if(dLo > 0&&dLa < 0){  
                angle= (90 -angle)+270;  
            } 
            v1000_debug("\r\n angle11 :%.1f \r\n",angle);
            
                                    
#else

            if(dLa > 0)//在南边
            {
               if(dLo> 0) //在西边   180--270 -=-- 西南
               {
                   angle = 180 + (90 - angle);
               }
               else if(dLo <= 0)//  东南    90- 180
               {
                   angle = 90 + (90 - angle);
               }
            }
           else if(dLa < 0 ) // 北边
           {
               if(dLo >= 0) //在西边   270--360  西北
               {
                   angle = 270 + (90 - angle);
               }
               else    // 东北
               {
                   angle = (90 - angle);
               }

           }
           else
           {

               if(dLo > 0) //在西边
               {
                   angle = 270 ;
               }
               else if(dLo < 0)  // 东
               {
                   angle = 90 ;
               }

           }
#endif
        }

        if((latitude1_flag=='S')&&(longitude1_flag=='E'))
        {
#if 1
                        
            if(dLo > 0&&dLa >= 0){  
                angle = (90-angle)+90;  
            }  
            else if(dLo <= 0&&dLa >= 0){  
                angle = angle+180.;  
            }else if(dLo < 0&&dLa < 0){  
                angle= (90 -angle)+270;  
            } 
            v1000_debug("\r\n angle11 :%.1f \r\n",angle);

                        
#else

            if(dLa > 0)//在南边
            {
               if(dLo < 0) //在西边   180--270 -=-- 西南
               {
                   angle = 180 + (90 - angle);
               }
               else if(dLo >= 0)//  东南    90- 180
               {
                   angle = 90 + (90 - angle);
               }
            }
           else if(dLa < 0 ) // 北边
           {
               if(dLo <= 0) //在西边   270--360  西北
               {
                   angle = 270 + (90 - angle);
               }
               else    // 东北
               {
                   angle = (90 - angle);
               }

           }
           else
           {

               if(dLo < 0) //在西边
               {
                   angle = 270 ;
               }
               else if(dLo > 0)  // 东
               {
                   angle = 90 ;
               }

           }
#endif
        }

        if((latitude1_flag=='N')&&(longitude1_flag=='E'))
        {

            v1000_debug("\r\n dla :%f \r\n",dLa);
            v1000_debug("\r\n dLo :%f \r\n",dLo);
            v1000_debug("\r\n angle :%.1f \r\n",angle);
#if 1

            if(dLo > 0&&dLa <= 0){  
                angle = (90-angle)+90;  
            }  
            else if(dLo <= 0&&dLa < 0){  
                angle = angle+180.;  
            }else if(dLo < 0&&dLa >= 0){  
                angle= (90 -angle)+270;  
            } 
            v1000_debug("\r\n angle11 :%.1f \r\n",angle);

#else
            if(dLa < 0)//在南边
            {
               if(dLo <= 0) //在西边   180--270 -=-- 西南
               {
                   angle = 180 + (90 - angle);
                    v1000_debug("\r\n angle11 :%.1f \r\n",angle);
               }
               else if(dLo > 0)//  东南    90- 180
               {
                   angle = 90 + (90 - angle);
                    v1000_debug("\r\n angle22 :%.1f \r\n",angle);
               }
            }
           else if(dLa > 0 ) // 北边
           {
               if(dLo <= 0) //在西边   270--360  西北
               {
                   angle = 270 + (90 - angle);
               }
               else    // 东北
               {
                   angle = (90 - angle);
               }

           }
           else
           {

               if(dLo < 0) //在西边
               {
                   angle = 270 ;
               }
               else if(dLo > 0)  // 东
               {
                   angle = 90 ;
               }

           }
#endif           
        }

        if((latitude1_flag=='N')&&(longitude1_flag=='W'))
        {

#if 1
            
            if(dLo <= 0&&dLa <= 0){  
                angle = (90-angle)+90;  
            }  
            else if(dLo > 0&&dLa < 0){  
                angle = angle+180.;  
            }else if(dLo >= 0&&dLa >= 0){  
                angle= (90 -angle)+270;  
            } 
            v1000_debug("\r\n angle11 :%.1f \r\n",angle);
            
#else

            if(dLa < 0)//在南边
            {
               if(dLo  > 0) //在西边   180--270 -=-- 西南
               {
                   angle = 180 + (90 - angle);
               }
               else if(dLo  <= 0)//  东南    90- 180
               {
                   angle = 90 + (90 - angle);
               }
            }
           else if(dLa > 0 ) // 北边
           {
               if(dLo >= 0) //在西边   270--360  西北
               {
                   angle = 270 + (90 - angle);
               }
               else    // 东北
               {
                   angle = (90 - angle);
               }

           }
           else
           {

               if(dLo > 0) //在西边
               {
                   angle = 270 ;
               }
               else if(dLo < 0)  // 东
               {
                   angle = 90 ;
               }

           }
#endif           
        }

    }


    //v1000_debug("\r\n angle222  %.1f",angle);

    return angle;

}


 #else
//如果两点的坐标分别为：东经20度，北纬45和西经160度，南纬45度,则输入"20 e 45 n 160 w 45 s"然后回车。
float get_angle(float latitude1, char latitude1_flag,float longitude1, char longitude1_flag,
                 float latitude2, char latitude2_flag,float longitude2, char longitude2_flag )
{

float shita1,shita2,fai1,fai2,x1,x2,y1,y2,z1,z2,x,y,z,angle;

   float dLo =longitude2 - longitude1;
   float dLa =latitude2 - latitude1;




    if(longitude1_flag=='W')
    {
        longitude1 = 360-longitude1;
    }
    if(latitude1_flag=='S')
    {
        latitude1 = -1*latitude1;
    }
    if(latitude2_flag=='S')
    {
        latitude2 = -1*latitude2;
    }
    if(longitude2_flag=='W')
    {
        longitude1 = 360-longitude1;
    }



     shita1 = longitude1/180*PI;
     shita2 = longitude2/180*PI;

   v1000_debug("\r\n shita1  %.1f",shita1);
   v1000_debug("\r\n shita2  %.1f",shita2);

     fai1 = PI/2-latitude1/180*PI;
     fai2 = PI/2-latitude2/180*PI;

   v1000_debug("\r\n fai1  %.1f",fai1);
   v1000_debug("\r\n fai2  %.1f",fai2);
   //   Ec = Rj + (Rc - Rj) * (90.- latitude1) / 90.;
    //  Ed = Ec * cos(fai2);


     x1 = sin(fai1)*cos(shita1);
     x2 = sin(fai2)*cos(shita2);
     y1 = sin(fai1)*sin(shita1);
     y2 = sin(fai2)*sin(shita2);
     z1 = cos(fai1);
     z2 = cos(fai2);

     x = x2-x1;
     y = y2-y1;
     z = z2-z1;

     angle = atan(z/sqrt(x*x + y*y));



   angle = angle/PI*180;

   v1000_debug("\r\n angle111  %.1f",angle);

 #if 0
    if(angle<0)
    {
        angle *= -1;
    }
    if(angle>90)
    {
        angle = 180-angle;
    }
#endif

  // angle = angle/PI*180;
    return angle;
}


#endif


 float GetAngle(float longitude1, char longitude1_flag, float latitude1, char latitude1_flag,
                 float longitude2, char longitude2_flag, float latitude2, char latitude2_flag)
 {
 float dRotateAngle = atan2(abs(longitude1- longitude2), abs(latitude1- latitude2));
 if (longitude2 >= longitude1)
 	{
 	if (latitude2 >= latitude1)
 		{
 		}
	else
		{
		dRotateAngle = PI - dRotateAngle;
		}
	}
 else
 	{
 	if (latitude2 >=latitude1)
		{
		dRotateAngle = 2 * PI - dRotateAngle;
		}
	else
		{
		dRotateAngle = PI + dRotateAngle;
		}
	}
 	dRotateAngle = dRotateAngle * 180 / PI;
	 v1000_debug("\r\n dRotateAngle  %.1f",dRotateAngle);
 return dRotateAngle;
 }


#if 0

//worm_Love sunHelper


typedef struct
{
//unsigned int jingdu;
//unsigned int jingfeng;
//unsigned int weidu;
//unsigned int weifeng;
float latitude;
float longitude;
unsigned int year;
u8 month;
u8 day;
u8 hour;
u8 mintue;
}data_input;

data_input data_in;
//	save_the_interest_place(25969118,'N',119022038,'E',0);
void getsunrise(void)
{

float n0;
float temp0;
float temp1;
unsigned long int total_day; //积日
float dz_jingdu;//经度订正
float dz_sk;//时刻订正
float temp2;
float degree_sun;
float Er,Ed,Et,Sd;
float SD_sun,h_sun,a_sun,a_sun_up,a_sun_down;
float temp3,temp4;

data_in.latitude =25.969118;
data_in.longitude=119.022038;
data_in.year=2006;
data_in.month=11;
data_in.day=7 ;
data_in.hour=6;
data_in.mintue=21;

n0 = 79.6764 + 0.2422 * (data_in.year-1985)-(int)((data_in.year- 1985)/4);
temp0= data_in.year/4.0-(int)(data_in.year/4);
temp1=32.8 ;
if( data_in.month <=2)
	temp1=30.6;
else
	{
	if(temp0==0)
		temp1=32.8 ;
	}
total_day = (int) (30.6 * data_in.month -temp1 + 0.5) +data_in.day ;

dz_jingdu = data_in.longitude;
dz_sk = data_in.hour-8+data_in.mintue/60.0 ;
temp2= total_day+(dz_sk -dz_jingdu)/24.0 ;
degree_sun=2*PI*( total_day-n0)/365.2422;

Er=1.000423 + 0.032359*sin(degree_sun)+ 0.000086*sin(2*degree_sun)- 0.008349*cos(degree_sun)+ 0.000115*cos(2*degree_sun);
Ed=0.3723 + 23.2567*sin(degree_sun)+ 0.1149*sin(2*degree_sun)-0.1712*sin(3*degree_sun)- 0.758*cos(degree_sun)+ 0.3656*cos(2*degree_sun)+ 0.0201*cos(3*degree_sun);
Et=0.0028 - 1.9857*sin(degree_sun)+ 9.9059*sin(2*degree_sun)- 7.0924*cos(degree_sun)- 0.6882*cos(2*degree_sun);
Sd=data_in.hour+(data_in.mintue-(120.0-data_in.longitude)*4)/60.0; //当地太阳时间


v1000_debug("\r\n Sd  %f",Sd);

SD_sun= (Sd+Et/60.0-12)*15;//当地太阳角度


v1000_debug("\r\n SD_sun  %f",SD_sun);

temp3=2*PI*(data_in.latitude)/360;
temp1=2*PI*Ed/360;
temp2=2*PI*SD_sun/360;
temp4=sin(temp1)*sin(temp3) +cos(temp1)*cos(temp3)*cos(temp2);
if(temp4>1)
   temp4=1;
else
   if(temp4<-1)
	temp4=-1;
h_sun=asin(temp4);//太阳高度角

temp4=(sin(h_sun)*sin(temp3)-sin(temp1))/(cos(h_sun)*cos(temp3));
if(temp4>1)
	temp4=1;
else
	if(temp4<-1)
		temp4=-1;
a_sun=acos(temp4);//太阳方位角

h_sun=h_sun*360/(2*PI);
a_sun=a_sun*360/(2*PI);

v1000_debug("\r\n h_sun  %f",h_sun);
v1000_debug("\r\n a_sun  %f",a_sun);

a_sun_up=acos(sin(temp1)/cos(temp3))*360/(2*PI);//日出方位
a_sun_down=a_sun_up-180; //日落方位


v1000_debug("\r\n a_sun_up  %f",a_sun_up);
v1000_debug("\r\n a_sun_down  %f",a_sun_down);

}
#endif



#ifndef PI
 #define PI        3.1415926535897932384
#endif

#define RADEG     ( 180.0 / PI )
#define DEGRAD    ( PI / 180.0 )

/* The trigonometric functions in degrees */

#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)
#define tand(x)  tan((x)*DEGRAD)

#define atand(x)    (RADEG*atan(x))
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atan2d(y,x) (RADEG*atan2(y,x))

#define days_since_2000_Jan_0(y,m,d) (367*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530)



int sunriset( int year, int month, int day, float lon, float lat,
                  float altit, int upper_limb, float *rise, float *set );

static void sunpos( float d, float *lon, float *r );

static void sun_RA_dec( float d, float *RA, float *dec, float *r );

static float revolution( float x );

static float rev180( float x );

static float GMST0( float d );

static void sunpos( float d, float *lon, float *r )
/******************************************************/
/* Computes the Sun's ecliptic longitude and distance */
/* at an instant given in d, number of days since     */
/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
/* computed, since it's always very near 0.           */
/******************************************************/
/******************************************************/
/* 计算太阳的黄道经度和距离							  */
/* 在d制定的瞬间，太阳的黄道纬度不计算				  */
/* 因为它无限接近于0								  */
/******************************************************/
{
      float M,			//太阳的近点距离
             w,         //黄道经度
                        //注：太阳的平黄径 = M + w
             e,         //地球轨道的偏移量
             E,         //偏近点角
             x, y,      //轨道的坐标
             v;         //真正的近点距离

      /* Compute mean elements */
	  //计算
      M = revolution( 356.0470 + 0.9856002585 * d );
      w = 282.9404 + 4.70935E-5 * d;
      e = 0.016709 - 1.151E-9 * d;

      /* Compute true longitude and radius vector */

      E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
            x = cosd(E) - e;
      y = sqrt( 1.0 - e*e ) * sind(E);
      *r = sqrt( x*x + y*y );              /* Solar distance */
      v = atan2d( y, x );                  /* True anomaly */
      *lon = v + w;                        /* True solar longitude */
      if ( *lon >= 360.0 )
            *lon -= 360.0;                   /* Make it 0..360 degrees */
}

static void sun_RA_dec( float d, float *RA, float *dec, float *r )
{
      float lon, obl_ecl, x, y, z;

      /* Compute Sun's ecliptical coordinates */
	  //计算太阳的黄道坐标
      sunpos( d, &lon, r );

      /* Compute ecliptic rectangular coordinates (z=0) */
      x = *r * cosd(lon);
      y = *r * sind(lon);

      /* Compute obliquity of ecliptic (inclination of Earth's axis) */
      obl_ecl = 23.4393 - 3.563E-7 * d;

      /* Convert to equatorial rectangular coordinates - x is unchanged */
      z = y * sind(obl_ecl);
      y = y * cosd(obl_ecl);

      /* Convert to spherical coordinates */
      *RA = atan2d( y, x );
      *dec = atan2d( z, sqrt(x*x + y*y) );

}  /* sun_RA_dec */


/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and &lt; 360.0                                   */
/******************************************************************/

#define INV360    ( 1.0 / 360.0 )

static float revolution( float x )
/*****************************************/
/* Reduce angle to within 0..360 degrees */
/*****************************************/
/*****************************************/
/* 把角度换算到0-360度之间				 */
/*****************************************/
{
      return( x - 360.0 * floor( x * INV360 ) );
}  /* revolution */

static float rev180( float x )
/*********************************************/
/* Reduce angle to within +180..+180 degrees */
/*********************************************/
/*********************************************/
/* 把角度换算到-180到180度之间				 */
/*********************************************/
{
      return( x - 360.0 * floor( x * INV360 + 0.5 ) );
}  /* revolution */


/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/
//取得0时的格林尼治时角
static float GMST0( float d )
{
      float sidtim0;
      /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
      /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
      /* add these numbers, I'll let the C compiler do it for me.  */
      /* Any decent C compiler will add the constants at compile   */
      /* time, imposing no runtime or code overhead.               */
      sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
                          ( 0.9856002585 + 4.70935E-5 ) * d );
      return sidtim0;
}  /* GMST0 */



int sunriset( int year, int month, int day, float lon, float lat,
                  float altit, int upper_limb, float *trise, float *tset )
/***************************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value IS critical in this function!            */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing rise/set     */
/*               times, and to zero when computing start/end of       */
/*               twilight.                                            */
/*        *rise = where to store the rise time                        */
/*        *set  = where to store the set  time                        */
/*                Both times are relative to the specified altitude,  */
/*                and thus this function can be used to compute       */
/*                various twilight times, as well as rise/set times   */
/* Return value:  0 = sun rises/sets this day, times stored at        */
/*                    *trise and *tset.                               */
/*               +1 = sun above the specified "horizon" 24 hours.     */
/*                    *trise set to time when the sun is at south,    */
/*                    minus 12 hours while *tset is set to the south  */
/*                    time plus 12 hours. "Day" length = 24 hours     */
/*               -1 = sun is below the specified "horizon" 24 hours   */
/*                    "Day" length = 0 hours, *trise and *tset are    */
/*                    both set to the time when the sun is at south.  */
/*                                                                    */
/**********************************************************************/
/***************************************************************************/
/* 中文注释: year,month,date = TIC日期, 范围为1801-2099年             */
/*       东经为正, 西经为负	北纬为正，南纬为负					      */
/*       经度的值是函数里的关键								          */
/*       altit = 日出日落时太阳对地平线的角距，请设置成-35/60度       */
/*         upper_limb: 计算日出日落时间设置为1				          */
/*        *rise = 保存日出时间					                      */
/*        *set  = 保存日落时间					                      */
/*                这些计算出来的日出日落时间都是在特定纬度的时间,	  */
/* Return value:  0 = 日出日落时间已经保存在*trise和*tset里           */
/*               +1 = 时间比该地点快了24个小时以上					  */
/*               -1 = 时间比该地点忙了24小时以上					  */
/*                                                                    */
/**********************************************************************/
{
      float  d,		//从2000年1月1日到现在的天数
      sr,			//日地距，天文学单位
      sRA,			//黄道经度
      sdec,			//太阳的倾角
      sradius,		//太阳的圆周半径
      t,			//每日的弧度(偏移量)
      tsouth,		//太阳在黄道南边的时间
      sidtime;		//本地恒星时

      int rc = 0;	//函数返回值，通常为0

    /* Compute d of 12h local mean solar time */
	  //计算d
      d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

      /* Compute local sidereal time of this moment */
	  //计算此刻的本地恒星时
      sidtime = revolution( GMST0(d) + 180.0 + lon );

      /* Compute Sun's RA + Decl at this moment */
	  //计算此刻太阳的黄道经度和太阳的倾角
      sun_RA_dec( d, &sRA, &sdec, &sr );

      /* Compute time when Sun is at south - in hours UT */
	  //计算太阳在黄道南边的时间
      tsouth = 12.0 - rev180(sidtime - sRA)/15.0;

      /* Compute the Sun's apparent radius, degrees */
	  //计算太阳的圆周半径
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitude altit: */
      {
            float cost;
            cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
                  ( cosd(lat) * cosd(sdec) );
            if ( cost >= 1.0 )
                  rc = -1, t = 0.0;       /* Sun always below altit */
            else if ( cost == -1.0 )
                  rc = +1, t = 12.0;      /* Sun always above altit */
            else
                  t = acosd(cost)/15.0;   /* The diurnal arc, hours */
      }

      /* Store rise and set times - in hours UT */
      *trise = tsouth - t;
      *tset  = tsouth + t;

      return rc;
}  /* __sunriset__ */

#define TRUE 1
#define FALSE 0

typedef struct SUN
{
	//char SunRise[2][10];
	//char SunSet[2][10];
	//char MoonRise[10];
	//char MoonSet[10];
	float dbMoonRise;
	float dbMoonSet;
//	float dbSunRise[2];//
//	float dbSunSet[2];
//	float dbDayTime;
//	float dbSunMoon;
//	float dbIllumTime;
	//char dayTime[10]; //昼长
	//char SunNoon[10]; //中午
	//char IllumTime[10]; //光照时间 天亮-天黑
}*PSunMoonTime,SunMoonTime;

float frac(float x)
{
	x -= (int)x;
	return( (x<0) ? x+1.0 : x );
}

float * minimoon(float t) {
// takes t and returns the geocentric ra and dec in an array mooneq
// claimed good to 5' (angle) in ra and 1' in dec
// tallies with another approximate method and with ICE for a couple of dates
	float p2 = 6.283185307, arc = 206264.8062, coseps = 0.91748, sineps = 0.39778;
	float L0, L, LS, F, D, H, S, N, DL, CB, L_moon, B_moon, V, W, X, Y, Z, RHO,dec,ra;
	static float mooneq[2];

	L0 = frac(0.606433 + 1336.855225 * t);	// mean longitude of moon
	L = p2 * frac(0.374897 + 1325.552410 * t); //mean anomaly of Moon
	LS = p2 * frac(0.993133 + 99.997361 * t); //mean anomaly of Sun
	D = p2 * frac(0.827361 + 1236.853086 * t); //difference in longitude of moon and sun
	F = p2 * frac(0.259086 + 1342.227825 * t); //mean argument of latitude

	// corrections to mean longitude in arcsec
	DL =  22640 * sin(L);
	DL += -4586 * sin(L - 2*D);
	DL += +2370 * sin(2*D);
	DL +=  +769 * sin(2*L);
	DL +=  -668 * sin(LS);
	DL +=  -412 * sin(2*F);
	DL +=  -212 * sin(2*L - 2*D);
	DL +=  -206 * sin(L + LS - 2*D);
	DL +=  +192 * sin(L + 2*D);
	DL +=  -165 * sin(LS - 2*D);
	DL +=  -125 * sin(D);
	DL +=  -110 * sin(L + LS);
	DL +=  +148 * sin(L - LS);
	DL +=   -55 * sin(2*F - 2*D);

	// simplified form of the latitude terms
	S = F + (DL + 412 * sin(2*F) + 541* sin(LS)) / arc;
	H = F - 2*D;
	N =   -526 * sin(H);
	N +=   +44 * sin(L + H);
	N +=   -31 * sin(-L + H);
	N +=   -23 * sin(LS + H);
	N +=   +11 * sin(-LS + H);
	N +=   -25 * sin(-2*L + F);
	N +=   +21 * sin(-L + F);

	// ecliptic long and lat of Moon in rads
	L_moon = p2 * frac(L0 + DL / 1296000);
	B_moon = (18520.0 * sin(S) + N) /arc;

	// equatorial coord conversion - note fixed obliquity
	CB = cos(B_moon);
	X = CB * cos(L_moon);
	V = CB * sin(L_moon);
	W = sin(B_moon);
	Y = coseps * V - sineps * W;
	Z = sineps * V + coseps * W;
	RHO = sqrt(1.0 - Z*Z);
	dec = (360.0 / p2) * atan(Z / RHO);
	ra = (48.0 / p2) * atan(Y / (X + RHO));
	if (ra <0 ) ra += 24;
	mooneq[1] = dec;
	mooneq[0] = ra;
	return mooneq;
}

float ipart(float x)
{//	returns the integer part - like int() in basic
	float a;
	if (x> 0) a = floor(x);
	else a = ceil(x);
	return a;
}

float range(float x)
{	//returns an angle in degrees in the range 0 to 360
	float a, b;
	b = x / 360;
	a = 360 * (b - ipart(b));
	if (a  < 0 )	a = a + 360;
	return a;
}

float lmst(float mjd, float glong)
{
	//  Takes the mjd and the longitude (west negative) and then returns
	//  the local sidereal time in hours. Im using Meeus formula 11.4
	//  instead of messing about with UTo and so on
	float lst, t, d;
	d = mjd - 51544.5;
	t = d / 36525.0;
	lst = range(280.46061837 + 360.98564736629 * d + 0.000387933 *t*t - t*t*t / 38710000);
	lst = lst/15.0 + glong/15.0;
	if (lst >= 24.0) lst -=24.0;
	return (lst);
}



float * minisun(float t)
{
	//  returns the ra and dec of the Sun in an array called suneq[]
	//  in decimal hours, degs referred to the equinox of date and using
	//  obliquity of the ecliptic at J2000.0 (small error for +- 100 yrs)
	//	takes t centuries since J2000.0. Claimed good to 1 arcmin
	float p2 = 6.283185307, coseps = 0.91748, sineps = 0.39778;
	float L, M, DL, SL, X, Y, Z, RHO, ra, dec;
	static float suneq[2];

	M = p2 * frac(0.993133 + 99.997361 * t);
	DL = 6893.0 * sin(M) + 72.0 * sin(2 * M);
	L = p2 * frac(0.7859453 + M / p2 + (6191.2 * t + DL)/1296000);
	SL = sin(L);
	X = cos(L);
	Y = coseps * SL;
	Z = sineps * SL;
	RHO = sqrt(1 - Z * Z);
	dec = (360.0 / p2) * atan(Z / RHO);
	ra = (48.0 / p2) * atan(Y / (X + RHO));
	if (ra <0 ) ra += 24;
	suneq[1] = dec;
	suneq[0] = ra;
	return suneq;
}


float sin_alt(int iobj, float mjd0, float hour, float glong, float cglat, float sglat)
{
	//  this rather mickey mouse function takes a lot of
	//  arguments and then returns the sine of the altitude of
	//  the object labelled by iobj. iobj = 1 is moon, iobj = 2 is sun
	float mjd, t, ra, dec, tau, salt, rads = 0.0174532925;
	float *objpos;
	mjd = mjd0 + hour/24.0;
	t = (mjd - 51544.5) / 36525.0;
	if (iobj == 1)	objpos = minimoon(t);
	else 	objpos = minisun(t);
	ra = objpos[0];
	dec = objpos[1];
	// hour angle of object
	tau = 15.0 * (lmst(mjd, glong) - ra);
	// sin(alt) of object using the conversion formulas
	salt = sglat * sin(rads*dec) + cglat * cos(rads*dec) * cos(rads*tau);
	return salt;
}

float * quad(float ym, float yz, float yp)
{
	//  finds the parabola throuh the three points (-1,ym), (0,yz), (1, yp)
	//  and returns the coordinates of the max/min (if any) xe, ye
	//  the values of x where the parabola crosses zero (roots of the quadratic)
	//  and the number of roots (0, 1 or 2) within the interval [-1, 1]
	//  well, this routine is producing sensible answers
	//  results passed as array [nz, z1, z2, xe, ye]
	float a, b, c, dis, dx, xe, ye, z1 = 0.0, z2 = 0.0, nz;
	static float quadout[5];

	nz = 0;
	a = 0.5 * (ym + yp) - yz;
	b = 0.5 * (yp - ym);
	c = yz;
	xe = -b / (2 * a);
	ye = (a * xe + b) * xe + c;
	dis = b * b - 4.0 * a * c;
	if (dis > 0)
	{
		dx = 0.5 * sqrt(dis) / fabs(a);
		z1 = xe - dx;
		z2 = xe + dx;
		if (fabs(z1) <= 1.0) nz += 1;
		if (fabs(z2) <= 1.0) nz += 1;
		if (z1 < -1.0) z1 = z2;
	}
	quadout[0] = nz;
	quadout[1] = z1;
	quadout[2] = z2;
	quadout[3] = xe;
	quadout[4] = ye;
	return quadout;
}

void find_moonrise_set(float mjd, float tz, float glong, float glat,int dls,int ST,PSunMoonTime moonTimes)
{
	//  Im using a separate function for moonrise/set to allow for different tabulations
	//  of moonrise and sun events ie weekly for sun and daily for moon. The logic of
	//  the function is identical to find_sun_and_twi_events_for_date()
	float  sglat, date, ym, yz,  utrise = 0.0, utset = 0.0,sinho,cglat,ye;
	float yp, nz,  hour, z1, z2,  rads = 0.0174532925;
	char rise, sett;
	float *quadout;

	sinho = sin(rads * 8/60);		//moonrise taken as centre of moon at +8 arcmin
	sglat = sin(rads * glat);
	cglat = cos(rads * glat);
	date = mjd - tz/24;
	rise = FALSE;
	sett = FALSE;
//	above = FALSE;
	hour = 1.0;
	ym = sin_alt(1, date, hour - 1.0, glong, cglat, sglat) - sinho;
//	if (ym > 0.0) above = TRUE;
	while(hour < 25 && (sett == FALSE || rise == FALSE))
	{
		yz = sin_alt(1, date, hour, glong, cglat, sglat) - sinho;
		yp = sin_alt(1, date, hour + 1.0, glong, cglat, sglat) - sinho;
		quadout = quad(ym, yz, yp);
		nz = quadout[0];
		z1 = quadout[1];
		z2 = quadout[2];
//		xe = quadout[3];
		ye = quadout[4];
		// case when one event is found in the interval
		if (nz == 1) {
			if (ym < 0.0) {
				utrise = hour + z1;
				rise = TRUE;
			}
			else {
				utset = hour + z1;
				sett = TRUE;
			}
		} // end of nz = 1 case

		// case where two events are found in this interval
		// (rare but whole reason we are not using simple iteration)
		if (nz == 2) {
			if (ye < 0.0) {
				utrise = hour + z2;
				utset = hour + z1;
			}
			else {
				utrise = hour + z1;
				utset = hour + z2;
			}
		}

		// set up the next search interval
		ym = yp;
		hour += 2.0;
	} // end of while loop

	if (rise == TRUE || sett == TRUE )
	{
		if (rise == TRUE)
		{
			if (ST == 0)moonTimes->dbMoonRise = utrise+dls;
			else moonTimes->dbMoonRise = lmst(mjd+(utrise-tz)/24, glong);
		}
		else    moonTimes->dbMoonRise = -1;
		if (sett == TRUE)
		{
			if (ST == 0)moonTimes->dbMoonSet = utset+dls;
			else moonTimes->dbMoonSet = lmst(mjd+(utset-tz)/24, glong);
		}
		else	moonTimes->dbMoonSet = -1;
	}
	else	moonTimes->dbMoonSet = -2;
}


#if 0
void find_sun_and_twi_events_for_date(float mjd,float tz, float glong,float  glat, int dls,int ST,PSunMoonTime SolTimes)
{
	//	this is my attempt to encapsulate most of the program in a function
	//	then this function can be generalised to find all the Sun events.
	float  sglat, date, ym, yz, utrise, utset, ye,xe,cglat;
	int j;
	float yp, nz, hour, z1, z2,  rads = 0.0174532925;
	char rise, sett,above;
	float * quadout;
	float sinho[4];

	sinho[0] = sin(rads * -0.833);		//sunset upper limb simple refraction
	sinho[1] = sin(rads *  -6.0);		//civil twi
	sinho[2] = sin(rads * -12.0);		//nautical twi
	sinho[3] = sin(rads * -18.0);		//astro twi
	sglat = sin(rads * glat);
	cglat = cos(rads * glat);
	date = mjd - tz/24;
	//main loop takes each value of sinho in turn and finds the rise/set
	//events associated with that altitude of the Sun
	for (j = 0; j < 2; j++)
	{
		rise = FALSE;
		sett = FALSE;
		above = FALSE;
		hour = 1.0;
		ym = sin_alt(2, date, hour - 1.0, glong, cglat, sglat) - sinho[j];
		if (ym > 0.0) above = TRUE;
		// the while loop finds the sin(alt) for sets of three consecutive
		// hours, and then tests for a single zero crossing in the interval
		// or for two zero crossings in an interval or for a grazing event
		// The flags rise and sett are set accordingly
		while(hour < 25 && (sett == FALSE || rise == FALSE))
		{
			yz = sin_alt(2, date, hour, glong, cglat, sglat) - sinho[j];
			yp = sin_alt(2, date, hour + 1.0, glong, cglat, sglat) - sinho[j];
			quadout = quad(ym, yz, yp);
			nz = quadout[0];
			z1 = quadout[1];
			z2 = quadout[2];
			xe = quadout[3];
			ye = quadout[4];
			// case when one event is found in the interval
			if (nz == 1)
			{
				if (ym < 0.0)
				{
					utrise = hour + z1;
					rise = TRUE;
				}
				else
				{
					utset = hour + z1;
					sett = TRUE;
				}
			} // end of nz = 1 case

			// case where two events are found in this interval
			// (rare but whole reason we are not using simple iteration)
			if (nz == 2) {
				if (ye < 0.0)
				{
					utrise = hour + z2;
					utset = hour + z1;
				}
				else
				{
					utrise = hour + z1;
					utset = hour + z2;
				}
			} // end of nz = 2 case

			// set up the next search interval
			ym = yp;
			hour += 2.0;

		} // end of while loop
		// now search has completed, we compile the string to pass back
		// to the main loop. The string depends on several combinations
		// of the above flag (always above or always below) and the rise
		// and sett flags
		if(j == 0)
		{
			//计算昼长
			//hrsmin(utset-utrise,(char *)&SolTimes->dayTime);
			SolTimes->dbDayTime=utset-utrise;
			//计算正午
			//hrsmin((utset+utrise)/2,(char *)&SolTimes->SunNoon);
			SolTimes->dbSunMoon=(utset+utrise)/2;
		}
		else
		{
			//计算光照
			//hrsmin(utset-utrise,(char *)&SolTimes->IllumTime);
			SolTimes->dbIllumTime=utset-utrise;
		}
		if (rise == TRUE || sett == TRUE )
		{
			if (rise == TRUE)
			{
				if (ST == 0)
					//hrsmin(utrise+dls,(char *)&SolTimes->SunRise[j]);
					SolTimes->dbSunRise[j]=utrise+dls;
				else
					//hrsmin(lmst(mjd+(utrise-tz)/24, glong),(char *)&SolTimes->SunRise[j]);
					SolTimes->dbSunRise[j]=lmst(mjd+(utrise-tz)/24,glong);
			}
			else    SolTimes->dbSunRise[j]=-1.0;
				//strcpy((char*)&SolTimes->SunRise[j],"----");
			if (sett == TRUE)
			{
			    if (ST == 0)
				//hrsmin(utset+dls,(char *)&SolTimes->SunSet[j]);
				SolTimes->dbSunSet[j]=utset+dls;
			    else
				//hrsmin(lmst(mjd+(utset-tz)/24, glong),(char *)&SolTimes->SunSet[j]);
				SolTimes->dbSunSet[j]=lmst(mjd+(utset-tz)/24,glong);
			}
			else    SolTimes->dbSunSet[j]=-1.0;
				//strcpy((char*)&SolTimes->SunSet[j],"--:--");
		}
		else {
			if (above == TRUE) {
				SolTimes->dbSunRise[j]=-2.0;
				SolTimes->dbSunSet[j]=-2.0;
				//strcpy((char*)&SolTimes->SunRise[j],"**:**");
				//strcpy((char*)&SolTimes->SunSet[j],"**:**");
			}
			else{
				SolTimes->dbSunRise[j]=-2.0;
				SolTimes->dbSunSet[j]=-2.0;
				//strcpy((char*)&SolTimes->SunRise[j],"**:**");
				//strcpy((char*)&SolTimes->SunSet[j],"**:**");
			}
		}
	} // end of for loop - next condition
	// return SolTimes;
}
#endif

float mjd(int day, int month, int year, int hour)
{
	//	Takes the day, month, year and hours in the day and returns the
	//  modified julian day number defined as mjd = jd - 2400000.5
	//  checked OK for Greg era dates - 26th Dec 02
	float a, b;
	if (month <= 2) {
		month = month + 12;
		year = year - 1;
	}
	a = 10000.0 * year + 100.0 * month + day;
	if (a <= 15821004.1)
		b = -2 * floor((year + 4716)/4) - 1179;
	else
		b = floor(year/400) - floor(year/100) + floor(year/4);
	a = 365.0 * year - 679004.0;
	return (a + b + floor(30.6001 * (month + 1)) + day + hour/24.0);
}


void GetSunMoonTime(float dbLon,float dbLat,int year,int month,int day,float TimeZone,PSunMoonTime pTime)
{
	float mjdd = mjd(day,month,year,0);

//	find_sun_and_twi_events_for_date(mjdd,TimeZone,dbLon,dbLat,0,0,pTime);
	find_moonrise_set(mjdd, TimeZone, dbLon,dbLat,0,0,pTime);
}


void moon_test(float dbLon,float dbLat,int year,int month,int day,float TimeZone)
{
    struct SUN SMTime;
    PSunMoonTime pTime=&SMTime;
    u8 hour,minute;
    u32 tp;
    extern __align(4) u8 dtbuf[50];   								//打印缓存器

    GetSunMoonTime(dbLon, dbLat, year, month, day, TimeZone, pTime);
    // v1000_debug("\r\n dbMoonRise  %.1f",pTime->dbMoonRise);
    // v1000_debug("\r\n dbMoonSet  %.1f",pTime->dbMoonSet);
    tp = pTime->dbMoonRise*1000;
	hour = (u8)(tp/1000);
	minute = (u8)((tp -hour*1000)*0.06);
    if(gpsx->fixmode!=3)
        v1000_debug((char *)dtbuf,"--:--");	//显示UTC日期
    else
        sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//显示UTC日期
	OLED_ShowString(22,32,dtbuf);

    tp = pTime->dbMoonSet*1000;
	hour = (u8)(tp/1000);
	minute = (u8)((tp -hour*1000)*0.06);
    if((gpsx->fixmode!=3)||(pTime->dbMoonSet<0.0))
       sprintf((char *)dtbuf,"--:--");	//显示UTC日期
    else
       sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//显示UTC日期
	OLED_ShowString(84,32,dtbuf);
}

