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
#include "cam2.h"

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




QString LC_cam2::name() const
 {
     return (tr("hole pattern"));
 }

PluginCapabilities LC_cam2::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("Info", tr("hole pattern"));
    return pluginCapabilities;
}

struct cam_listelem_struct
{
   int typ;			       /* 1=line, 3=circle, 4=arc */
   int cw;			       /* clockwise */
   double x1, y1, x2, y2, len, wi, r, w1, w2, ccx, ccy;
} cam_listelem;

// -------------
 
QString print_cam_line ( cam_listelem_struct el, int index )
{
   QString out;
 
   
       switch (el.typ) 
     {	
      case DPI::CIRCLE:
      case DPI::ARC:
	       	
	
	
	if (index == 0)                        /* start der kontur */
	  {	
	     out.append( QString ( "\nLBL 3456  "));
	     out.append( QString ( "\nQ216 = "));
	     out.append( QString ( my_sprintf(el.x1, 1)));
	     out.append( QString ( " ; x \nQ217 =  "));
	     out.append( QString ( my_sprintf(el.y1, 1)));
	     out.append( QString ( " ; y \nL x+q216 y+q217  z+22  r0  f max  m13 "));
	     out.append( QString ( "\nL  z+2  f max \nm8 \nm99 ; ("));
	     
	  }
	else
	  {
             out.append( QString ( "\nQ216 = "));
	     out.append( QString ( my_sprintf(el.x1, 1)));
	     out.append( QString ( " ; x \nQ217 =  "));
	     out.append( QString ( my_sprintf(el.y1, 1)));
	     out.append( QString ( " ; y \nL x+q216 y+q217  f max  m99 ; ("));
	     
	  }
	
	     out.append( QString ( my_sprintf(index + 1, 1) ));
	     out.append( QString ( ") \n; . "));
			 
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

void LC_cam2::execComm(Document_Interface *doc,
                             QWidget *parent, QString cmd)
{
   Q_UNUSED(parent);
   Q_UNUSED(cmd);
   
 //  QPointF base1;
   
   QList<Plug_Entity *> obj;
   
   bool yes  = doc->getSelect(&obj);
    if (!yes || obj.isEmpty()) return;

//   yes = doc->getPoint(&base1, QString("starte an point:"));

   /*  dialogbox ???  */

   
   
   int list_len = obj.size();
   
   cam_listelem_struct  cam_liste1[list_len];
   
   int count_circles = 0;
   
   int j;
   for (j = 0; j < obj.size(); ++j) 
     {
	QHash<int, QVariant> data2;
	obj.at(j)->getData(&data2);
	
	int et = data2.value(DPI::ETYPE).toInt();
	
	cam_liste1[j].typ = et;
	
	//specific entity data
	switch (et) 
	  {
	     
	   case DPI::CIRCLE:
	   case DPI::ARC:
	     
	     // if (ae->getAngleLength() < 4.66)   /* ca. 267 grad (knapp 3/4 kreis) */
	       		
	     /* 
	      if (isReversed()) 
	      {
	      if (data.angle1<data.angle2) 
	      {
	      ret = data.angle1+2*M_PI-data.angle2;
	      }
	      else 
	      {
	      ret = data.angle1-data.angle2;
	      }
	      }
	      else 
	      {
	      if (data.angle2<data.angle1) 
	      {
	      ret = data.angle2+2*M_PI-data.angle1;
	      }
	      else 
	      {
	      ret = data.angle2-data.angle1;
	      }
	      }
	      // full circle:
	      if (fabs(ret)<1.0e-6) {
	      ret = 2*M_PI;
	      }
	      */
	     
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
	       
	     break;
	   default:
	     break;

	  }

 //	d(cam_liste1[j], j, "execComm");
	
     }				       /* for (j = 0; j < obj.size(); ++j) */
   
   
   
      
   // ----------------

   
   QString text;
      
   int i;
   for (i = 0; i < count_circles; ++i) 
     {        
	QString cam_data = print_cam_line ( cam_liste1[i], i );
	
	text.append(cam_data);
    }
   
   if(1)			       /* end */
     {
	text.append( QString ( "\nM9"));
	text.append( QString ( "\nL  z+22  f max "));
        text.append( QString ( "\nLBL 0 "));
     }
     
   
   // ----------------------------
   
    lc_cam2dlg dlg(parent);
    dlg.setText(text);
    dlg.exec();

    while (!obj.isEmpty())
        delete obj.takeFirst();
}


/*****************************/

lc_cam2dlg::lc_cam2dlg(QWidget *parent) :  QDialog(parent)
{
    setWindowTitle(tr(" hole pattern "));
//    QTextEdit *edit= new QTextEdit(this);
    edit.setReadOnly (true);
    edit.setAcceptRichText ( false );
    QDialogButtonBox* bb = new QDialogButtonBox( QDialogButtonBox::Close, Qt::Horizontal, this );
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&edit);
    mainLayout->addWidget(bb);
    this->setLayout(mainLayout);
    this->resize ( 550, 780 );

    connect(bb, SIGNAL(rejected()), this, SLOT(accept()));
}


void lc_cam2dlg::setText(QString text)
{
    edit.setText(text);
}
lc_cam2dlg::~lc_cam2dlg()
{
}

Q_EXPORT_PLUGIN2(lc_cam2, LC_cam2);
