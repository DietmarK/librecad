/*****************************************************************************/
/* cam1  aehnlich  list.cpp - List selected entities                                        */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/


#include <QTextEdit>
#include <QColor>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <math.h>
#include "document_interface.h"
#include "cam1.h"

#include  <stdio.h>
#include <iostream> 

// char *my_sprintf (double, int = 0);
  
   char *my_sprintf (double d, int plus = 0)	       
     {
	/* tausender-separator entfernen, Komma als decimal-point character, Nachnullen entfernen */
	/* wenn plus == 1 dann ohne '+'-vorzeichen */
	
	static char num[222];
	int i,k, len;
	
	strcpy (num, "");
	
	if (plus)
	  sprintf (num, "%.3f", d);
	else
	  sprintf (num, "%+.3f", d);
	
	len = strlen (num);
	
	for (k=0; k<len; k++)
	  {
	     if (num[k] == ',')
	       { 
		  for (i=k; i<len; i++)
		    {
		       num[i] = num[i+1];
		    }
		  num[len] = '\0';
	       }
	     else if (num[k] == '.')
	       num[k] = ',';
	  }

	
	
	if (strchr(num, ','))
	  {		     
	     len = strlen (num);
	     
	     for (k=len-1; k>0; k--)
	       {
		  if ( num[k] == ',')
		    {
		       num[k] = '\0';
		       break;
		    }
		  else if (num[k] == '0')
		    num[k] = '\0';
		  else
		    break;	     
	       }
	  }
	
	return num;
     }
   
// ------------------------------------

int abs_diff (double a, double b)
{
   double c;
   
   c = fabs ( a - b );
   
   if ( c > 0.003 )		       /*  max. abweichung zum letzten punkt */
     return 1;
   else
     return 0;
   
}

   
// ------------------------------------




QString LC_cam1::name() const
 {
     return (tr("outline"));
 }

PluginCapabilities LC_cam1::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("Info", tr("outline"));
    return pluginCapabilities;
}

struct cam_listelem_struct
{
   int typ;			       /* 1=line, 3=circle, 4=arc */
   int cw;			       /* clockwise */
   double x1, y1, x2, y2, len, wi, r, w1, w2, ccx, ccy;
} cam_listelem;

// -------------
 
int search_in_cam_liste (double x, double y, cam_listelem_struct *liste, int l, int& ch)
{
   int k;
   int a = -1;
   
   for (k = 0; k < l; ++k)
     {
	if (liste[k].typ && 0 == abs_diff(x, liste[k].x1) && 0 == abs_diff(y, liste[k].y1) )
	  {
	     a = k;
	     ch = 0;
	     break;
	  }
     }
   if (a == -1)
     {
	for (k = 0; k < l; ++k)
	  {
	     if (liste[k].typ && 0 == abs_diff(x, liste[k].x2) && 0 == abs_diff(y, liste[k].y2) )
	       {
		  a = k;
		  ch = 1;
		  break;
	       }
	  }
	
     }
   return a;
}

// -------------

int copy_element (int i1, int i2, cam_listelem_struct *liste1, cam_listelem_struct *liste2, int ch)
{
   if (i1 < 0)
     return i2;
   
   liste2[i2].typ = liste1[i1].typ;
   liste2[i2].r = liste1[i1].r;
   liste2[i2].ccx = liste1[i1].ccx;
   liste2[i2].ccy = liste1[i1].ccy;
   liste2[i2].len = liste1[i1].len;
   
   if (ch)			       /* enden tauschen */
     {
	liste2[i2].x1 = liste1[i1].x2;
	liste2[i2].y1 = liste1[i1].y2;
	liste2[i2].x2 = liste1[i1].x1;
	liste2[i2].y2 = liste1[i1].y1;
        liste2[i2].wi = liste1[i1].wi - 180;
	if (liste2[i2].wi < 0)
	  liste2[i2].wi += 360;
	liste2[i2].w1 = liste1[i1].w2;
	liste2[i2].w2 = liste1[i1].w1;
	if (liste1[i1].cw)
	  liste2[i2].cw = 0;
	else
	  liste2[i2].cw = 1;
	
     }
   else
     {
	liste2[i2].x1 = liste1[i1].x1;
	liste2[i2].y1 = liste1[i1].y1;
	liste2[i2].x2 = liste1[i1].x2;
	liste2[i2].y2 = liste1[i1].y2;
	liste2[i2].w1 = liste1[i1].w1;
	liste2[i2].w2 = liste1[i1].w2;
	liste2[i2].cw = liste1[i1].cw;	     
     }

   liste1[i1].typ = 0; 		       /* "gelesen" */
   
   return ++i2;
}


// -------------

QString print_cam_line ( cam_listelem_struct el, int index )
{
   QString out;
 
   
       switch (el.typ) 
     {	
      case DPI::LINE:
	       	
	if (index == 0)                        /* start der kontur */
	  {	
	     out.append( QString ( "\nL  x"));
	     out.append( QString ( my_sprintf(el.x1)));
	     out.append( QString ( "  y"));
	     out.append( QString ( my_sprintf(el.y1)));
	     out.append( QString ( "  rl  fq2  m8  "));
	  }
	
	out.append( QString ( "\nL  x"));
	out.append( QString ( my_sprintf(el.x2)));
	out.append( QString ( "  y"));
	out.append( QString ( my_sprintf(el.y2)));
	out.append( QString ( "  ; ("));

	out.append( QString ( my_sprintf(index + 1, 1) ));
	out.append( QString ( ") "));
   
	break;	
      case DPI::ARC:

	out.append( QString ( "\nCC  x"));
	out.append( QString ( my_sprintf(el.ccx)));
	out.append( QString ( "  y"));
	out.append( QString ( my_sprintf(el.ccy)));
	
	if (index)
	  {
	     out.append( QString ( "  ; r="));
	     out.append( QString ( my_sprintf(el.r)));
	  }
	
	
	if (index == 0)           
	  {             /* start der kontur */
	     out.append( QString ( "\nLP  pr"));
	     out.append( QString ( my_sprintf(el.r)));
	     out.append( QString ( "  pa"));
	     out.append( QString ( my_sprintf(el.w1)));
	     out.append( QString ( "  rl  fq2 m8 "));
	  }
	  {
	     out.append( QString ( "\nCP  pa"));
	     out.append( QString ( my_sprintf(el.w2)));
	     
	     if  (el.cw)
	       out.append( QString ( "  dr-  ; ("));
	     else
	       out.append( QString ( "  dr+  ; ("));
	  }
	
	out.append( QString ( my_sprintf(index + 1, 1) ));
	out.append( QString ( ") "));
	
	
	break;
        
      default:
	break;
     }
   
   return out;
   
}

// -------------

void d (cam_listelem_struct e, int j, const char *text = "")	
{	     
   std::cerr 
     << text 
     << " index=" << j
     << "  typ=" << e.typ 
     << "  x1=" << e.x1 
     << "  y1=" << e.y1 
     << "  x2=" << e.x2 
     << "  y2=" << e.y2 
     << "  ccx=" << e.ccx 
     << "  ccy=" << e.ccy
     << "  r=" <<  e.r 
     << "  w1=" << e.w1 
     << "  w2=" << e.w2 
     << "  cw=" << e.cw 
     << "\n";
}
	
	
// ------------------

void LC_cam1::execComm(Document_Interface *doc,
                             QWidget *parent, QString cmd)
{
   Q_UNUSED(parent);
   Q_UNUSED(cmd);
   
   QPointF base1;
   
   QList<Plug_Entity *> obj;
   
   bool yes  = doc->getSelect(&obj);
    if (!yes || obj.isEmpty()) return;

   yes = doc->getPoint(&base1, QString("starte an point:"));

   if (0) 
     {
	std::cerr << " x: " << base1.x() << "    y: " << base1.y() << "\n";
     };

   int list_len = obj.size();
   
   cam_listelem_struct  cam_liste1[list_len];
   
   int count_circles = 0;
   int circle_1 = -1, circle_2 = -1;	       /* index merken */
   int circle_begin = -2;
   int circle_end = -2;
   
   int j;
   for (j = 0; j < obj.size(); ++j) 
     {
	QHash<int, QVariant> data2;
	obj.at(j)->getData(&data2);
	
	int et = data2.value(DPI::ETYPE).toInt();
	
	cam_liste1[j].typ = et;
	
	double x1, y1, x2, y2, len, wi, r, w1, w2, ccx, ccy;
	
	
	//specific entity data
	switch (et) 
	  {
	     
	   case DPI::LINE:
	     
	     x1 = data2.value(DPI::STARTX).toDouble();
	     y1 = data2.value(DPI::STARTY).toDouble();
	     x2 = data2.value(DPI::ENDX).toDouble();
	     y2 = data2.value(DPI::ENDY).toDouble();
	            
	     len = sqrt( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) );
	     
	     wi = asin((y2 - y1) / len) * 180 / M_PI;
	     
	     if (x2 - x1 < 0)
	       wi = 180 - wi;
	     if (wi < 0)
	       wi += 360;
	     
	     cam_liste1[j].x1 = x1;
	     cam_liste1[j].y1 = y1;
	     cam_liste1[j].x2 = x2;
	     cam_liste1[j].y2 = y2;
	     cam_liste1[j].wi = wi;
	     cam_liste1[j].len = len;

	     cam_liste1[j].r = 0;
	     cam_liste1[j].w1 = 0;
	     cam_liste1[j].w2 = 0;
	     cam_liste1[j].ccx = 0;
	     cam_liste1[j].ccy = 0;
	     cam_liste1[j].cw = 0;

	     break;
	   case DPI::ARC:
	     
	     ccx = data2.value(DPI::STARTX).toDouble();
	     ccy = data2.value(DPI::STARTY).toDouble();
	     r = data2.value(DPI::RADIUS).toDouble();
	     w1 = data2.value(DPI::STARTANGLE).toDouble();
	     w2 = data2.value(DPI::ENDANGLE).toDouble();
	     
	     x1 = ccx + r * cos(w1);
	     y1 = ccy + r * sin(w1);
	     x2 = ccx + r * cos(w2);
	     y2 = ccy + r * sin(w2);
	     
	     cam_liste1[j].x1 = x1;
	     cam_liste1[j].y1 = y1;
	     cam_liste1[j].x2 = x2;
	     cam_liste1[j].y2 = y2;
	     cam_liste1[j].w1 = w1 * 180 / M_PI;
	     cam_liste1[j].w2 = w2 * 180 / M_PI;
	     cam_liste1[j].r = r;
	     cam_liste1[j].ccx = ccx;
	     cam_liste1[j].ccy = ccy;
	     cam_liste1[j].cw = data2.value(DPI::REVERSED).toInt();
	     
	     cam_liste1[j].len = 0;
	     cam_liste1[j].wi = 0;
	     
             break;
	   case DPI::CIRCLE:
	     
		  cam_liste1[j].x1 = data2.value(DPI::STARTX).toDouble();
		  cam_liste1[j].y1 = data2.value(DPI::STARTY).toDouble();
	     
	     cam_liste1[j].len = 0;
	     cam_liste1[j].wi = 0;
	     cam_liste1[j].x2 = 0;
	     cam_liste1[j].y2 = 0;
	     cam_liste1[j].r = 0;
	     cam_liste1[j].w1 = 0;
	     cam_liste1[j].w2 = 0;
	     cam_liste1[j].ccx = 0;
	     cam_liste1[j].ccy = 0;
	     cam_liste1[j].cw = 0;

		  count_circles++;
	     if (count_circles == 1)
	       circle_1 = j;
	     if (count_circles == 2)
	       circle_2 = j;
	     
	       
	     break;
	   default:
	     break;

	  }

//	d(cam_liste1[j], j, "execComm");
	
     }				       /* for (j = 0; j < obj.size(); ++j) */
   
   
   
      
   // ----------------

   cam_listelem_struct  cam_liste2[list_len];   /* sortiert, anfang <--> ende bei bedarf getauscht */
   
   int change = 0;		       /* anfang <--> ende  ??  */

   /* first element: */
   
   int m, n;
   
   m = search_in_cam_liste (base1.x(), base1.y(), cam_liste1, list_len, change );
	
//   std::cerr << " m = " << m << "    change: " << change  << "\n";
   
   int o = 0; 			       /* index liste2 */
   
	n = copy_element (m, o, cam_liste1, cam_liste2, change);
   
   //         d(cam_liste1[m], m, "cam_liste1");
   //       d(cam_liste2[o], o, "cam_liste2");
   
   
   // -----------------  die restlichen elemente kopieren:
   
   int p;
   for (p = 0; p < list_len; ++p)
     {
	m = search_in_cam_liste (cam_liste2[o].x2, cam_liste2[o].y2, cam_liste1, list_len, change );

	if (m < 0)		       /* kein treffer */
	  break;
	
	o = n;
	
	n = copy_element (m, o, cam_liste1, cam_liste2, change);

//	std::cerr << "\n for (p ... :  m = " << m << "  o = " << o << "    change: " << change  << "\n";
	
		
//	 d(cam_liste1[m], m, "cam_liste1");
//	 d(cam_liste2[o], o, "cam_liste2");
	
	  
     }

   
   // ----------------------------------
   
   QString text;
   
   if (count_circles)
     {
	circle_begin = circle_1;
	
	if(count_circles > 1)	       /* found 2 circles, or more ...  */
	  {	     
	     circle_end = circle_2;
	     	     
	     double xa, ya, x1, y1, x2, y2, a1, a2;
	     
	     xa = base1.x();
	     ya = base1.y();
	     
	     x1 = cam_liste1[circle_1].x1;
	     y1 = cam_liste1[circle_1].y1;
	     
	     x2 = cam_liste1[circle_2].x1;
	     y2 = cam_liste1[circle_2].y1;
	     
	     a1 = (x1-xa)*(x1-xa) + (y1-ya)*(y1-ya);   /* x^2 + y^2 */
	     a2 = (x2-xa)*(x2-xa) + (y2-ya)*(y2-ya);
	       
	     if (a2 < a1)	       /* have to change */
	       {		  
		  circle_begin = circle_2;
		  circle_end = circle_1;
	       }
	  }			       /* if(count_circles > 1) */
	
	if(circle_begin > -1)	       /* eintauchpunkt */
	  {
	     text.append( QString ( "\nQ1 = 999 ; z "));
	     text.append( QString ( "\nQ2 = 0 ; f "));
	     text.append( QString ( "\nLBL 1234 "));
	     text.append( QString ( "\nL  x"));
	     text.append( QString ( my_sprintf(cam_liste1[circle_begin].x1) ) ) ;
	     text.append( QString ( "  y")) ;
	     text.append( QString ( my_sprintf(cam_liste1[circle_begin].y1) ) ) ;
	     text.append( QString ( "  z+22  r0  f max  m13 ; eintauchpunkt" ) ) ;
	     text.append( QString ( "\nL  z+2  f max ")) ;
	     text.append( QString ( "\nL  z+q1  f2222 ")) ;
	     
	  }
     }
   
   
   
   int i;
   for (i = 0; i < n; ++i) 
     {        
	QString cam_data = print_cam_line ( cam_liste2[i], i );
	
	text.append(cam_data);
    }
   
   
   if (circle_end > -1)		       /* wegfahr-punkt */
     {
	text.append( QString ( "\nM9"));
	text.append( QString ( "\nL  x"));
	text.append( QString ( my_sprintf(cam_liste1[circle_end].x1)));
	text.append( QString ( "  y"));
	text.append( QString ( my_sprintf(cam_liste1[circle_end].y1)));
	text.append( QString ( "  r0 ; wegfahren "));
	text.append( QString ( "\nL  z+22  f max "));
        text.append( QString ( "\nLBL 0 "));
     }
   
   
   // ----------------------------
   
    lc_cam1dlg dlg(parent);
    dlg.setText(text);
    dlg.exec();

    while (!obj.isEmpty())
        delete obj.takeFirst();
}


/*****************************/

lc_cam1dlg::lc_cam1dlg(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr(" outline "));
//    QTextEdit *edit= new QTextEdit(this);
    edit.setReadOnly (true);
    edit.setAcceptRichText ( false );
    QDialogButtonBox* bb = new QDialogButtonBox( QDialogButtonBox::Close, Qt::Horizontal, this );
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&edit);
    mainLayout->addWidget(bb);
    this->setLayout(mainLayout);
    this->resize ( 550, 550 );

    connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}


void lc_cam1dlg::setText(QString text)
{
    edit.setText(text);
}
lc_cam1dlg::~lc_cam1dlg()
{
}

Q_EXPORT_PLUGIN2(lc_cam1, LC_cam1);
