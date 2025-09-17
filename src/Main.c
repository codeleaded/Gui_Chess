//#define IMAGE_PNG_ONLY

#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Files.h"
#include "/home/codeleaded/System/Static/Library/TransformedView.h"
#include "/home/codeleaded/System/Static/Library/AudioPlayer.h"
#include "/home/codeleaded/System/Static/Library/Chess.h"


TransformedView tv;
ChessBoard cb;
Vic2 pos;
ChessFigure* selected;
AudioPlayer ap;

void Setup(AlxWindow* w){
	AlxFont_Resize(&window.AlxFont,16,16);

	tv = TransformedView_Make(
		(Vec2){ GetWidth(),GetHeight() * 0.125f },
		(Vec2){ 0.0f,0.0f },
		(Vec2){ 0.125f,0.125f },
		(float)GetWidth() / (float)GetHeight()
	);

	cb = ChessBoard_New();
	pos = (Vic2){ -1,-1 };
	selected = NULL;

	//ap = AudioPlayer_New();
	//AudioPlayer_Start(&ap);
}
void Update(AlxWindow* w){
	TransformedView_Output(&tv,(Vec2){ GetWidth(),GetHeight() });
	//TransformedView_HandlePanZoom(&tv,window.Strokes,GetMouse());
	TransformedView_Border(&tv,(Rect){ { 0.0f,0.0f },{ cb.boardsize.x,cb.boardsize.y } });

	const Vec2 wMouse = TransformedView_ScreenWorldPos(&tv,(Vec2){ w->MouseX,w->MouseY });
	const Vec2 dPiece = TransformedView_WorldScreenPos(&tv,(Vec2){ 1.0f,1.0f });

	if(Stroke(ALX_MOUSE_L).PRESSED){
		pos.x = (int)wMouse.x;
		pos.y = (int)wMouse.y;
		if(ChessBoard_Select(&cb,pos)){
			selected = ChessBoard_Get(&cb,pos.x,pos.y);
		}else{
			selected = NULL;
		}
	}
	if(Stroke(ALX_MOUSE_L).RELEASED){
		if(selected){
			if(ChessBoard_MoveTo(&cb,selected,pos,(Vic2){ (int)wMouse.x,(int)wMouse.y })){
				ChessBoard_Set(&cb,(unsigned int)wMouse.x,(unsigned int)wMouse.y,selected);
				*selected = ChessFigure_New(CHESSID_NONE);
			}

			pos = (Vic2){ -1,-1 };
			selected = NULL;
		}
	}

	Clear(RED);

	ChessBoard_Reload(&cb,dPiece.x,dPiece.y);
	for(int y = 0;y<CHESSBOARD_SIZE;y++){
		for(int x = 0;x<CHESSBOARD_SIZE;x++){
			const Vec2 p = TransformedView_WorldScreenPos(&tv,(Vec2){ x,y });
			char bw = (x % 2 && (y + 1) % 2) || ((x + 1) % 2 && y % 2);
			RenderRect(p.x,p.y,dPiece.x,dPiece.y,(bw ? WHITE : BLACK));
			
			const ChessFigure* cf = ChessBoard_Get(&cb,x,y);
			
			if(cf && selected != cf){
				SubSprite ss = ChessBoard_SubSprite(&cb,cf->id);

				if(ss.sp)
					RenderSubSpriteAlpha(ss.sp,p.x,p.y,ss.ox,ss.oy,ss.dx,ss.dy);
			}
		}
	}
	
	if(selected){
		SubSprite ss = ChessBoard_SubSprite(&cb,selected->id);
		RenderSubSpriteAlpha(ss.sp,(GetMouse().x - ss.dx * 0.5f),(GetMouse().y - ss.dy * 0.5f),ss.ox,ss.oy,ss.dx,ss.dy);
	}

	String str = String_Format("S:%f,%f",tv.Scale.x,tv.Scale.y);
	CStr_RenderSizeAlxFont(WINDOW_STD_ARGS,&window.AlxFont,str.Memory,str.size,0.0f,0.0f,WHITE);
	String_Free(&str);
}
void Delete(AlxWindow* w){
	ChessBoard_Free(&cb);
	//AudioPlayer_Free(&ap);
}

int main(){
    if(Create("Mario",1200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}