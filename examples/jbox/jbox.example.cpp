
#include "maxcpp6.h"

// inherit from the MSP base class, template-specialized for myself:

class Example : public JboxCpp6<Example>{
public:
	t_jrgba attr_bgcolor;
	t_jrgba attr_textcolor;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// constructor :
	Example(){
		setupIO(1,1);
		setupJbox(JBOX_DRAWFIRSTIN);
		setupJbox(JBOX_DRAWBACKGROUND);
		setupJbox(JBOX_GROWBOTH);
		// setupJbox(JBOX_DRAWINLAST);
		// setupJbox(JBOX_NODRAWBOX);
		// setupJbox(JBOX_TRANSPARENT);			
		// setupJbox(JBOX_MOUSEDRAGDELTA);
		// setupJbox(JBOX_BACKGROUND);
		// setupJbox(JBOX_NOGROW);
		// setupJbox(JBOX_GROWY);
		// setupJbox(JBOX_TEXTFIELD);
		attr_bgcolor.red = attr_bgcolor.green = attr_bgcolor.blue = 0.0;
		attr_textcolor.red = attr_textcolor.green = attr_bgcolor.alpha = 1.0;
		attr_textcolor.alpha = attr_textcolor.blue = 1.0 ;
	}

	~Example(){}

	// inlet methods :
	void bang(long inlet) { 
		post("bang in inlet %i!", inlet); 
	}

	// notify :
	t_max_err notify(symbol *s, symbol *msg, void *sender, void *data){
		if(msg == gensym("attr_modified")){
			symbol *name = (symbol *)object_method((object *)data,gensym("getname"));

			jbox_redraw((t_jbox *)this);
		}
		return jbox_notify((t_jbox *)this, s, msg, sender, data);
	}

	// mouse :
	void mousedown(t_object *patcherview, t_pt pt, long modifiers){

	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// paint :
	void paint(t_jgraphics *g, t_object *view , t_rect rect){
		t_jfont 		*font;
		t_jtextlayout 	*textlayout;
		char 			text[16];
		
		// draw rectangle
		jgraphics_rectangle_rounded(g, 0, 0, rect.width, rect.height, 0.25, 0.25);
		jgraphics_set_source_jrgba(g, &attr_bgcolor);	
		jgraphics_fill(g);	

		// draw text
		font = jfont_create(jbox_get_fontname((t_object *)this)->s_name, (t_jgraphics_font_slant)jbox_get_font_slant((t_object *)this), (t_jgraphics_font_weight)jbox_get_font_weight((t_object *)this), jbox_get_fontsize((t_object *)this));
		textlayout = jtextlayout_create();

		sprintf(text,"%s","jbox.example");

		jtextlayout_set(textlayout, text, font, 0, 0, rect.width , rect.height, JGRAPHICS_TEXT_JUSTIFICATION_CENTERED, JGRAPHICS_TEXTLAYOUT_NOWRAP);

		jtextlayout_settextcolor(textlayout, &attr_textcolor); 
		jtextlayout_draw(textlayout, g);
		jtextlayout_destroy(textlayout);
		jfont_destroy(font);
	}	

};

int C74_EXPORT main(void){

	Example::makeMaxClass("jbox.example",/*JBOX_COLOR |*/ JBOX_FIXWIDTH | JBOX_FONTATTR);

	// methods
	REGISTER_METHOD(Example, bang);
	REGISTER_METHOD_MOUSE(Example, mousedown);

	// TO DO - ALIASES for ATTR registation
	CLASS_STICKY_ATTR(Example::m_class, "category", 0, "Color");

	CLASS_ATTR_RGBA(Example::m_class, "bgcolor", 0, Example, attr_bgcolor); 
	CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(Example::m_class, "bgcolor", 0, "0.28 0.84 1 0.4"); 
	CLASS_ATTR_STYLE_LABEL(Example::m_class,"bgcolor",0,"rgba","Background Color");
	
	CLASS_ATTR_RGBA(Example::m_class, "textcolor", 0, Example, attr_textcolor); 
	CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(Example::m_class, "textcolor", 0, "0.83 0.83 0.83 1"); 
	CLASS_ATTR_STYLE_LABEL(Example::m_class,"textcolor",0,"rgba","Text Color");

	CLASS_STICKY_ATTR_CLEAR(Example::m_class, "category");

	return 0;
}

