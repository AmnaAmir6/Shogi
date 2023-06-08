#include<windows.h>
#include<conio.h>
#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;

enum Turn { White, Black };

#define DBLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGREY 7
#define DARKGREY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define DWHITE 15


void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}

void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}

void SetClr(int clr)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}

void DrawBox(int sr, int sc, int Brows, int Bcols, int color)
{
	char Sym = -37;
	for (int ri = 0; ri < Brows; ri++)
	{
		for (int ci = 0; ci < Bcols; ci++)
		{
			SetClr(color);
			gotoRowCol(sr + ri, sc + ci);
			cout << Sym;
		}
	}
}

void DrawBorder(int sr, int sc, int Brows, int Bcols, int color)
{
	char Sym = -37;
	for (int ri = 0; ri < Brows; ri++)
	{
		for (int ci = 0; ci < Bcols; ci++)
		{
			SetClr(color);
			if (ri == 0 || ri == Brows - 1 || ci == 0 || ci == Brows - 1)
			{
				gotoRowCol(sr + ri, sc + ci);
				cout << Sym;
			}
		}
	}
}

struct Position
{
	int ri, ci;
};

void SelectPos(Position& P, int dim)
{
	int prows, pcols;
	getRowColbyLeftClick(prows, pcols);
	int mr = prows / 8;
	int mc = pcols / 8;
	P.ri = mr;
	P.ci = mc;
}

void startGame(char& c)
{
	_getch();
	system("cls");
	DrawBox(40, 48, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 48);
	cout << "   New Game ";
	DrawBox(40, 78, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 78);
	cout << "   Old Game ";
	Position P;
	int ri, ci;
	SelectPos(P, 9);
	P.ri = P.ri * 9, P.ci = P.ci * 9;
	if (P.ci >= 48 && P.ci <= 63)
	{
		c = 'S';
	}
	else
	{
		c = 'O';
	}
	SetClr(15);
	system("cls");
}

void TurnMsg(string PNames)
{
	gotoRowCol(75, 0);
	cout << PNames << "'s Turn" << endl;
}

bool isValid(Position P, int dim)
{
	return P.ri >= 0 && P.ri < dim && P.ci >= 0 && P.ci < dim;
}

bool IsMyPiece(char C, int Turn)
{
	if (Turn == White)
		return C >= 'A' && C <= 'Z';
	else
		return C >= 'a' && C <= 'z';
}

bool clickedUndo(Position Sc)
{
	int ci = Sc.ci * 8 + 5, ri = Sc.ri * 8 + 4;
	if (ci >= 145 && ci <= 165 && ri >= 75 && ri <= 85)
		return true;
	return false;
}

bool IsValidScMain(char** B, Position Sc, int dim, int Turn, char* WCP, char* BCP)
{
	if (clickedUndo(Sc))
		return true;
	if (Sc.ri <= 4 && Sc.ci>= 11)
	{
		int x = Sc.ci - 11;
		return IsMyPiece(WCP[x], Turn);
	}
	else if (Sc.ri >= 5 && Sc.ci >= 11)
	{
		int x = Sc.ci - 11;
		return IsMyPiece(BCP[x], Turn);
	}
	else
	{
		return IsMyPiece(B[Sc.ri][Sc.ci], Turn);
	}
}

bool IsValidSc(char** B, Position Sc, int dim, int Turn)
{
	if (!isValid(Sc, dim))
		return false;
	return IsMyPiece(B[Sc.ri][Sc.ci], Turn);
}

bool IsValidDc(char** B, Position Dc, int dim, int Turn)
{
	if (!isValid(Dc, dim))
		return false;
	return !(IsMyPiece(B[Dc.ri][Dc.ci], Turn));
}

void UpdateBoard(char**& B, Position Sc, Position Dc)
{
	char C;
	C = B[Sc.ri][Sc.ci];
	B[Sc.ri][Sc.ci] = '-';
	B[Dc.ri][Dc.ci] = C;
}

void UpdateBoardMain(char**& B, Position Sc, Position Dc,char* WCP,char* BCP)
{
	char C;
	if (Sc.ri <= 4 && Sc.ci >= 11)
	{
		int i = Sc.ci - 11;
		if (Sc.ri == 3)
			i = i + 9;
		C = WCP[i];
	}
	else if (Sc.ri <= 5 && Sc.ci >= 11)
	{
		int i = Sc.ci - 11;
			if (Sc.ri == 6)
				i = i + 9;
			C = BCP[i];
	}
	else
	{
		C = B[Sc.ri][Sc.ci];
	}
	B[Sc.ri][Sc.ci] = '-';
	B[Dc.ri][Dc.ci] = C;
}

void ChangeTurn(int& Turn)
{
	Turn = (Turn + 1) % 2;
}

void PrintPosition(Position P)
{
	cout << P.ri + 1 << char(P.ci + 65);
}

bool isHorizontal(Position Sc, Position Dc)
{
	return Dc.ri == Sc.ri;
}

bool isVertical(Position Sc, Position Dc)
{
	return Dc.ci == Sc.ci;
}

bool isDiagnol(Position Sc, Position Dc)
{
	int dr = abs(Sc.ri - Dc.ri);
	int dc = abs(Sc.ci - Dc.ci);
	return dr == dc;
}

bool isHorizonPathClear(char** B, Position Sc, Position Dc)
{
	if (Sc.ci < Dc.ci)
	{
		for (int ci = Sc.ci + 1; ci < Dc.ci; ci++)
		{
			if (B[Sc.ri][ci] != '-')
				return false;
		}
	}
	else
	{
		for (int ci = Sc.ci - 1; ci > Dc.ci; ci--)
		{
			if (B[Sc.ri][ci] != '-')
				return false;
		}
	}
	return true;
}

bool isVerticalPathClear(char** B, Position Sc, Position Dc)
{
	if (Sc.ri < Dc.ri)
	{
		for (int ri = Sc.ri + 1; ri < Dc.ri; ri++)
		{
			if (B[ri][Sc.ci] != '-')
				return false;
		}
	}
	else
	{
		for (int ri = Sc.ri - 1; ri > Dc.ri; ri--)
		{
			if (B[ri][Sc.ci] != '-')
				return false;
		}
	}
	return true;
}

bool isDiagonalPathClearL2R(char** B, Position Sc, Position Dc)
{
	if (Sc.ri < Dc.ri && Sc.ci < Dc.ci)
	{
		for (int ri = Sc.ri + 1, ci = Sc.ci + 1; ci < Dc.ci; ci++, ri++)
		{
			if (B[ri][ci] != '-')
				return false;
		}
	}
	else if (Sc.ri > Dc.ri && Sc.ci > Dc.ci)
	{
		for (int ri = Sc.ri - 1, ci = Sc.ri - 1; ri > Dc.ri; ri--, ci--)
		{
			if (B[ri][ci] != '-')
				return false;
		}
	}
	return true;
}

bool isDiagonalPathClearR2L(char** B, Position Sc, Position Dc)
{
	if (Sc.ri < Dc.ri && Sc.ci > Dc.ci)
	{
		for (int ri = Sc.ri + 1, ci = Sc.ci - 1; ri < Dc.ri; ri++, ci--)
		{
			if (B[ri][ci] != '-')
				return false;
		}
	}
	else if (Sc.ri > Dc.ri && Sc.ci < Dc.ci)
	{
		for (int ri = Sc.ri - 1, ci = Sc.ri + 1; ri > Dc.ri; ri--, ci++)
		{
			if (B[ri][ci] != '-')
				return false;
		}
	}
	return true;
}

bool isL2RDiagnol(Position Sc, Position Dc)
{
	int dr = Sc.ri - Dc.ri;
	int dc = Sc.ci - Dc.ci;
	if (dr == dc)
		return true;
	return false;
}

bool BishopLegality(char** B, Position Sc, Position Dc)
{
	if (isDiagnol(Sc, Dc))
	{
		if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	return false;
}

bool WhitePawnLegality(Position Sc, Position Dc)
{
	return isVertical(Sc, Dc) && Sc.ri - Dc.ri == 1;
}

bool BlackPawnLegality(Position Sc, Position Dc)
{
	return isVertical(Sc, Dc) && Sc.ri - Dc.ri == -1;
}

bool KingLegality(char** B, Position Sc, Position Dc)
{
	if (isHorizontal(Sc, Dc) && abs(Sc.ci - Dc.ci) == 1)
		return isHorizonPathClear(B, Sc, Dc);
	else if (isDiagnol(Sc, Dc) && abs(Sc.ri - Dc.ri) == 1 && abs(Sc.ci - Dc.ci) == 1)
	{
		if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	else
		return isVertical(Sc, Dc) && isVerticalPathClear(B, Sc, Dc) && abs(Sc.ri - Dc.ri) == 1;
}

bool WhiteGoldGeneralLegality(char** B, Position Sc, Position Dc)
{
	if (isHorizontal(Sc, Dc) && abs(Sc.ci - Dc.ci) == 1)
		return isHorizonPathClear(B, Sc, Dc);
	else if (isDiagnol(Sc, Dc) && abs(Sc.ri - Dc.ri) == 1)
	{
		if ((Sc.ri < Dc.ri && Sc.ci > Dc.ci) || (Sc.ri < Dc.ri && Sc.ci < Dc.ci))
			return false;
		else if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	else
		return isVertical(Sc, Dc) && isVerticalPathClear(B, Sc, Dc) && abs(Sc.ri - Dc.ri) == 1;
}

bool BlackGoldGeneralLegality(char** B, Position Sc, Position Dc)
{
	if (isHorizontal(Sc, Dc) && abs(Sc.ci - Dc.ci) == 1)
		return isHorizonPathClear(B, Sc, Dc);
	else if (isDiagnol(Sc, Dc) && abs(Sc.ri - Dc.ri) == 1)
	{
		if ((Sc.ri > Dc.ri && Sc.ci < Dc.ci) || (Sc.ri > Dc.ri && Sc.ci > Dc.ci))
			return false;
		else if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	else
		return isVertical(Sc, Dc) && isVerticalPathClear(B, Sc, Dc) && abs(Sc.ri - Dc.ri) == 1;
}

bool WhiteLaceLegality(char** B, Position Sc, Position Dc)
{
	return isVertical(Sc, Dc) && isVerticalPathClear(B, Sc, Dc) && Sc.ri - Dc.ri > 0;
}

bool BlackLaceLegality(char** B, Position Sc, Position Dc)
{
	return isVertical(Sc, Dc) && isVerticalPathClear(B, Sc, Dc) && Sc.ri - Dc.ri < 0;
}

bool WhiteSilverGeneralLegality(char** B, Position Sc, Position Dc)
{
	if (isVertical(Sc, Dc) && Sc.ri - Dc.ri == 1)
		return true;
	else if (isDiagnol(Sc, Dc) && abs(Sc.ri - Dc.ri) == 1)
	{
		if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	else
		return false;
}

bool BlackSilverGeneralLegality(char** B, Position Sc, Position Dc)
{
	if (isVertical(Sc, Dc) && Sc.ri - Dc.ri == -1)
		return true;
	else if (isDiagnol(Sc, Dc) && abs(Sc.ri - Dc.ri) == 1)
	{
		if (isL2RDiagnol(Sc, Dc))
			return isDiagonalPathClearL2R(B, Sc, Dc);
		else
			return isDiagonalPathClearR2L(B, Sc, Dc);
	}
	else
		return false;
}

bool WhiteKnightLegality(char** B, Position Sc, Position Dc)
{
	return Sc.ri - Dc.ri == 2 && abs(Sc.ci - Dc.ci) == 1;
}

bool BlackKnightLegality(char** B, Position Sc, Position Dc)
{
	return Sc.ri - Dc.ri == -2 && abs(Sc.ci - Dc.ci) == 1;
}

bool RockLegality(char** B, Position Sc, Position Dc)
{
	if (isVertical(Sc, Dc))
		return isVerticalPathClear(B, Sc, Dc);
	else if (isHorizontal(Sc, Dc))
		return isHorizonPathClear(B, Sc, Dc);
	else
		return false;
}

Position findKing(char** B, int dim, int Turn)
{
	Position P;
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			if (B[ri][ci] == 'K' || B[ri][ci] == 'k')
			{
				if (IsMyPiece(B[ri][ci], Turn))
				{
					P.ri = ri;
					P.ci = ci;
					return P;
				}
			}
		}
	}
}

void DuplicateBoard(char**& DB, char** B, int dim)
{
	DB = new char* [dim] {};
	for (int i = 0; i < dim; i++)
		DB[i] = new char[dim] {};
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			DB[ri][ci] = B[ri][ci];
		}
	}
}

bool PromotedBlackSilverKnightLancePawn(char** B, Position Sc, Position Dc)
{
	return BlackGoldGeneralLegality(B, Sc, Dc);
}

bool PromotedWhiteSilverKnightLancePawn(char** B, Position Sc, Position Dc)
{
	return WhiteGoldGeneralLegality(B, Sc, Dc);
}

bool PromotedBishop(char** B, Position Sc, Position Dc)
{
	return BishopLegality(B, Sc, Dc) || KingLegality(B, Sc, Dc);
}

bool PromotedRock(char** B, Position Sc, Position Dc)
{
	return RockLegality(B, Sc, Dc) || KingLegality(B, Sc, Dc);
}

bool usingCaptured(Position Sc, int dim)
{
	return Sc.ci > dim;
}

bool isLegalMove(char** B, Position Sc, Position Dc, int* PPr, int* PPc)
{
	if (clickedUndo(Sc))
		return true;
	if (usingCaptured(Sc, 9))
	{
		if (B[Dc.ri][Dc.ci] != '-')
			return false;
		else
			return true;
	}
	bool isPromoted = false;
	for (int i = 0; i < 40; i++)
	{
		if (Sc.ri == PPr[i] && Sc.ci == PPc[i])
			isPromoted = true;
	}
	if (isPromoted == true)
	{
		switch (B[Sc.ri][Sc.ci])
		{
		case 'L':
		case 'N':
		case 'P':
		case 'S':
			return PromotedWhiteSilverKnightLancePawn(B, Sc, Dc);
		case 'l':
		case 'n':
		case 'p':
		case 's':
			return PromotedBlackSilverKnightLancePawn(B, Sc, Dc);
		case 'B':
		case 'b':
			return PromotedBishop(B, Sc, Dc);
		case 'R':
		case 'r':
			return PromotedRock(B, Sc, Dc);
		default:
			return false;
		}
	}
	else
	{
		switch (B[Sc.ri][Sc.ci])
		{
		case 'L':
			return WhiteLaceLegality(B, Sc, Dc);
		case'l':
			return BlackLaceLegality(B, Sc, Dc);
		case 'S':
			return WhiteSilverGeneralLegality(B, Sc, Dc);
		case 's':
			return BlackSilverGeneralLegality(B, Sc, Dc);
		case 'G':
			return WhiteGoldGeneralLegality(B, Sc, Dc);
		case'g':
			return BlackGoldGeneralLegality(B, Sc, Dc);
		case 'P':
			return WhitePawnLegality(Sc, Dc);
		case 'p':
			return BlackPawnLegality(Sc, Dc);
		case 'B':
		case'b':
			return BishopLegality(B, Sc, Dc);
		case 'R':
		case 'r':
			return RockLegality(B, Sc, Dc);
		case 'N':
			return WhiteKnightLegality(B, Sc, Dc);
		case 'n':
			return BlackKnightLegality(B, Sc, Dc);
		case 'K':
		case 'k':
			return KingLegality(B, Sc, Dc);
		default:
			return false;
		}
	}
}

bool Check(char** B, int dim, int Turn, int* PPr, int* PPc)
{
	ChangeTurn(Turn);
	Position PDc = findKing(B, dim, Turn);
	ChangeTurn(Turn);
	Position PSc;
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			PSc.ri = ri;
			PSc.ci = ci;
			if (IsValidSc(B, PSc, dim, Turn) && isLegalMove(B, PSc, PDc, PPr, PPc))
				return true;
		}
	}
	return false;
}

bool SelfCheck(char** B, int dim, int Turn, int* PPr, int* PPc)
{
	ChangeTurn(Turn);
	return Check(B, dim, Turn, PPr, PPc);
}

bool CheckMate(char** DB, char** B, int dim, int Turn, int* PPr, int* PPc)
{
	bool CM = true;
	ChangeTurn(Turn);
	Position Sc = findKing(B, dim, Turn);
	Position Dc;
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			Dc.ri = ri;
			Dc.ci = ci;
			char c = B[Dc.ri][Dc.ci];
			UpdateBoard(DB, Sc, Dc);
			if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc) && !SelfCheck(DB, dim, Turn, PPr, PPc))
				CM = false;
			if (Dc.ri == Sc.ri && Dc.ci == Sc.ci)
			{
				if (!SelfCheck(DB, dim, Turn, PPr, PPc))
					CM = false;
			}
			DB[Sc.ri][Sc.ci] = DB[Dc.ri][Dc.ci];
			DB[Dc.ri][Dc.ci] = c;
		}
	}
	return CM;
}

bool isLegalPawnDropCol(char** B, Position Dc, int dim, int Turn,int* PPr,int* PPc)
{
	if (Turn == White)
	{
		for (int r = 0; r < dim; r++)
		{
			if (B[r][Dc.ci] == 'P')
			{
				bool found = false;
				for (int i = 0; i < 40; i++)
				{
					if (r == PPr[i] && Dc.ci == PPc[i])
						found = true;
				}
				if (found == false)
					return false;
			}
		}
	}
	else
	{
		for (int r = 0; r < dim; r++)
		{
			if (B[r][Dc.ci] == 'p')
			{
				bool found = false;
				for (int i = 0; i < 40; i++)
				{
					if (r == PPr[i] && Dc.ci == PPc[i])
						found = true;
				}
				if (found == false)
					return false;
			}
		}
	}
	return true;
}

bool isLegalSilverGeneralMove(char** B, Position Dc, int dim)
{
	for (int c = 0; c < dim; c++)
	{
		if (B[Dc.ri][c] == 's' || B[Dc.ri][c] == 'S' || B[Dc.ri][c] == 'g' || B[Dc.ri][c] == 'G')
			return false;
	}
	for (int r = 0; r < dim; r++)
	{
		if (B[r][Dc.ci] == 's' || B[r][Dc.ci] == 'S' || B[r][Dc.ci] == 'g' || B[r][Dc.ci] == 'G')
			return false;
	}
	return true;
}

bool isLegalMoveMain(char** B, Position Sc, Position Dc, int* PPr, int* PPc,int Turn,char* WCP,char* BCP,char** DB,int dim)
{
	if (clickedUndo(Sc))
		return true;
	if (usingCaptured(Sc, 9))
	{
		if (B[Dc.ri][Dc.ci] != '-')
			return false;
		int pi = Sc.ci - 11;
		if (Sc.ri == 3 || Sc.ci == 6)
		{
			pi = pi + 8;
		}
		char c;
		if (Turn == White)
		{
			c = WCP[pi];
		}
		else
		{
			c = BCP[pi];
		}
		if (c == 'p' || c == 'l')
		{
			if (Dc.ri == 8)
				return false;
		}
		if (c == 'P' || c == 'L')
		{
			if (Dc.ri == 0)
				return false;
		}
		if (c == 'n')
		{
			if (Dc.ri > 6)
				return false;
		}
		if (c == 'N')
		{
			if (Dc.ri < 2)
				return false;
		}
		if (c == 'p' || c == 'P')
		{
			UpdateBoardMain(DB, Sc, Dc, WCP, BCP);
			if(CheckMate(DB,B,dim,Turn,PPr,PPc))
				return false;
			if (!isLegalPawnDropCol(B, Dc, dim, Turn, PPr, PPc))
				return false;
		}
		if (c == 's' || c == 'S')
		{
			if (!isLegalSilverGeneralMove(B, Dc, dim))
				return false;
		}
		return true;
	}	
	bool isPromoted = false;
	for (int i = 0; i < 40; i++)
	{
		if (Sc.ri == PPr[i] && Sc.ci == PPc[i])
			isPromoted = true;
	}
	if (isPromoted == true)
	{
		switch (B[Sc.ri][Sc.ci])
		{
		case 'L':
		case 'N':
		case 'P':
		case 'S':
			return PromotedWhiteSilverKnightLancePawn(B, Sc, Dc);
		case 'l':
		case 'n':
		case 'p':
		case 's':
			return PromotedBlackSilverKnightLancePawn(B, Sc, Dc);
		case 'B':
		case 'b':
			return PromotedBishop(B, Sc, Dc);
		case 'R':
		case 'r':
			return PromotedRock(B, Sc, Dc);
		default:
			return false;
		}
	}
	else
	{
		switch (B[Sc.ri][Sc.ci])
		{
		case 'L':
			return WhiteLaceLegality(B, Sc, Dc);
		case'l':
			return BlackLaceLegality(B, Sc, Dc);
		case 'S':
			return WhiteSilverGeneralLegality(B, Sc, Dc);
		case 's':
			return BlackSilverGeneralLegality(B, Sc, Dc);
		case 'G':
			return WhiteGoldGeneralLegality(B, Sc, Dc);
		case'g':
			return BlackGoldGeneralLegality(B, Sc, Dc);
		case 'P':
			return WhitePawnLegality(Sc, Dc);
		case 'p':
			return BlackPawnLegality(Sc, Dc);
		case 'B':
		case'b':
			return BishopLegality(B, Sc, Dc);
		case 'R':
		case 'r':
			return RockLegality(B, Sc, Dc);
		case 'N':
			return WhiteKnightLegality(B, Sc, Dc);
		case 'n':
			return BlackKnightLegality(B, Sc, Dc);
		case 'K':
		case 'k':
			return KingLegality(B, Sc, Dc);
		default:
			return false;
		}
	}
}

void unhighlight(char** B, bool** PM, int dim,Position Sc)
{
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			if (PM[ri][ci] == true)
			{
				int sr = ri * 8;
				int sc = ci * 8;
				if ((ri + ci) % 2 == 0)
				{
					DrawBorder(sr, sc, 8, 8, 7);
				}
				else
					DrawBorder(sr, sc, 8, 8, 8);
			}
		}
	}
	int sr = Sc.ri * 8;
	int sc = Sc.ci * 8;
	if (Sc.ci > dim && Sc.ri < 4)
		sr += 2, sc += 2;
	else if (Sc.ci > dim && Sc.ri > 4)
		sr -= 2, sc += 2;
	if ((Sc.ri + Sc.ci) % 2 == 0)
	{
		DrawBorder(sr, sc, 8, 8, 7);
	}
	else
		DrawBorder(sr, sc, 8, 8, 8);
	SetClr(15);
	gotoRowCol(8 * dim, 0);
}

bool** ComputeHighlight(char** B, Position Sc, int dim, int Turn, int* PPr, int* PPc, char** DB,char* WCP,char* BCP)
{
	bool** PM = new bool* [dim] {};
	for (int i = 0; i < dim; i++)
	{
		PM[i] = new bool[dim] {};
	}
	Position Dc;
	char a;
	if (usingCaptured(Sc, dim))
	{
		int pi = Sc.ci - 11;
		if (Sc.ri == 3 || Sc.ci == 6)
		{
			pi = pi + 8;
		}
		char c;
		if (Turn == White)
		{
			c = WCP[pi];
		}
		else
		{
			c = BCP[pi];
		}
		if (c == 'p' || c == 'l')
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri;
					Dc.ci = ci;
					if (Dc.ri != 8)
					{
						if (c == 'p')
						{
							a = DB[Dc.ri][Dc.ci];
							UpdateBoardMain(DB, Sc, Dc, WCP, BCP);
							if (IsValidDc(B, Dc, dim, Turn) && !CheckMate(DB, B, dim, Turn, PPr, PPc) && isLegalPawnDropCol(B, Dc, dim, Turn, PPr, PPc))
							{
								if(B[Dc.ri][Dc.ci] == '-')
									PM[ri][ci] = true;
							}
							BCP[pi] = c;
							DB[Dc.ri][Dc.ci] = a;
						}
						else
						{
							PM[ri][ci] = true;
						}
					}
				}
			}
		}
		else if (c == 'P' || c == 'L')
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri;
					Dc.ci = ci;
					if (Dc.ri != 0)
					{
						if (c == 'P')
						{
							a = DB[Dc.ri][Dc.ci];
							UpdateBoardMain(DB, Sc, Dc, WCP, BCP);
							if (IsValidDc(B, Dc, dim, Turn) && !CheckMate(DB, B, dim, Turn, PPr, PPc) && isLegalPawnDropCol(B, Dc, dim, Turn, PPr, PPc))
							{
								if (B[Dc.ri][Dc.ci] == '-')
									PM[ri][ci] = true;
							}
							WCP[pi] = c;
							DB[Dc.ri][Dc.ci] = a;
						}
						else
						{
							PM[ri][ci] = true;
						}
					}
				}
			}
		}
		else if (c == 'n')
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri;
					Dc.ci = ci;
					if (Dc.ri <= 6)
					{
						if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
						{
							PM[ri][ci] = true;
						}
					}
				}
			}
		}
		else if (c == 'N')
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri;
					Dc.ci = ci;
					if (Dc.ri >= 2)
					{
						if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
						{
							PM[ri][ci] = true;
						}
					}
				}
			}
		}
		else if (c == 's' || c == 'S')
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri;
					Dc.ci = ci;
					if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
					{
						if (isLegalSilverGeneralMove(B, Dc, dim))
						{
							PM[ri][ci] = true;
						}
					}
				}
			}
		}
		else
		{
			for (int ri = 0; ri < dim; ri++)
			{
				for (int ci = 0; ci < dim; ci++)
				{
					Dc.ri = ri, Dc.ci = ci;
					if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
					{
						PM[ri][ci] = true;
					}
				}
			}
		}
	}
	else if (SelfCheck(B, dim, Turn, PPr, PPc))
	{
		for (int ri = 0; ri < dim; ri++)
		{
			for (int ci = 0; ci < dim; ci++)
			{
				Dc.ri = ri;
				Dc.ci = ci;
				char c = DB[Dc.ri][Dc.ci];
				UpdateBoard(DB, Sc, Dc);
				if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
				{
					if (!SelfCheck(DB, dim, Turn, PPr, PPc))
						PM[ri][ci] = true;
				}
				DB[Sc.ri][Sc.ci] = DB[Dc.ri][Dc.ci];
				DB[Dc.ri][Dc.ci] = c;
			}
		}
	}
	else
	{
		for (int ri = 0; ri < dim; ri++)
		{
			for (int ci = 0; ci < dim; ci++)
			{
				Dc.ri = ri, Dc.ci = ci;
				if (IsValidDc(B, Dc, dim, Turn) && isLegalMove(B, Sc, Dc, PPr, PPc))
				{
					PM[ri][ci] = true;
				}
			}
		}
	}
	return PM;
}

void highlight(char** B, bool** PM, int dim,Position Sc)
{
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			if (PM[ri][ci] == true)
			{
				int sr = ri * 8;
				int sc = ci * 8;
				if (B[ri][ci] != '-')
					DrawBorder(sr, sc, 8, 8, 4);
				else
					DrawBorder(sr, sc, 8, 8, 13);
			}
		}
	}
	int sr = Sc.ri * 8;
	int sc = Sc.ci * 8;
	if (Sc.ci > dim && Sc.ri < 4)
		sr += 2, sc += 2;
	else if (Sc.ci > dim && Sc.ri > 4)
		sr -= 2, sc += 2;
	DrawBorder(sr, sc, 8, 8, 10);
	SetClr(15);
	gotoRowCol(8 * dim, 0);
}

bool isPromotable(Position P, int Turn, int* PPr, int* PPc)
{
	bool Promoted = false;
	for (int i = 0; i < 40; i++)
	{
		if (P.ri == PPr[i] && P.ci == PPc[i])
			Promoted = true;
	}
	if (Promoted == false)
	{
		if (Turn == White)
		{
			if (P.ri < 3)
				return true;
		}
		else
		{
			if (P.ri > 5)
				return true;
		}
	}
	return false;
}

void undoBoard(char*** &UndoB,int &UndoI, char** B,int dim, int& Turn, int* PPr, int* PPc, char* WCP, char* BCP, int wi, int bi, int i, int**& UndoPPr, int**& UndoPPc, char**& UndoWCP, char**& UndoBCP,int*& Undowi,int*& Undobi,int*& Undoi)
{
	for (int r = 0; r < dim; r++)
	{
		for (int c = 0; c < dim; c++)
		{
			UndoB[UndoI][r][c] = B[r][c];
		}
	}
	for (int i = 0; i < 40; i++)
	{
		UndoPPr[UndoI][i] = PPr[i];
	}
	for (int i = 0; i < 40; i++)
	{
		UndoPPc[UndoI][i] = PPc[i];
	}
	for (int i = 0; i < 20; i++)
	{
		UndoWCP[UndoI][i] = WCP[i];
	}
	for (int i = 0; i < 20; i++)
	{
		UndoBCP[UndoI][i] = BCP[i];
	}
	Undowi[UndoI] = wi;
	Undobi[UndoI] = bi;
	Undoi[UndoI] = i;
	UndoI++;
}

void Undo(char***& UndoB, int& UndoI, char**& B, int dim, int& Turn, int*& PPr, int*& PPc, char*& WCP, char*& BCP, int& wi, int& bi, int& i, int**& UndoPPr, int**& UndoPPc, char**& UndoWCP, char**& UndoBCP, int*& Undowi, int*& Undobi, int*& Undoi)
{
	for (int r = 0; r < dim; r++)
	{
		for (int c = 0; c < dim; c++)
		{
			B[r][c]= UndoB[UndoI - 1][r][c] ;
		}
	}
	for (int i = 0; i < 40; i++)
	{
		PPr[i] = UndoPPr[UndoI - 1][i];
	}
	for (int i = 0; i < 40; i++)
	{
		PPc[i] = UndoPPc[UndoI - 1][i];
	}
	for (int i = 0; i < 20; i++)
	{
		WCP[i] = UndoWCP[UndoI - 1][i];
	}
	for (int i = 0; i < 20; i++)
	{
		BCP[i] = UndoBCP[UndoI - 1][i];
	}
	wi = Undowi[UndoI - 1];
	bi = Undobi[UndoI - 1];
	i = Undoi[UndoI - 1];
	if (UndoI % 2 == 0)
		Turn = White;
	else
		Turn = Black;
	UndoI--;
}

void PromotedPieceMove(Position Sc, Position Dc, int*& PPr, int*& PPc)
{
	for (int i = 0; i < 40; i++)
	{
		if (Sc.ri == PPr[i] && Sc.ci == PPc[i])
		{
			PPr[i] = Dc.ri;
			PPc[i] = Dc.ci;
		}
	}
}

void isCaptured(char** B, Position Dc, int Turn, char*& WCP, char*& BCP, int& wi, int& bi, int*& PPr, int*& PPc)
{
	if (B[Dc.ri][Dc.ci] != '-')
	{
		for (int i = 0; i < 40; i++)
		{
			if (PPr[i] == Dc.ri && PPc[i] == Dc.ci)
				PPr[i] = 0,PPc[i] = 0;
		}
		if (Turn == White)
		{
			B[Dc.ri][Dc.ci] = toupper(B[Dc.ri][Dc.ci]);
			WCP[wi] = B[Dc.ri][Dc.ci];
			wi++;
		}
		else
		{
			B[Dc.ri][Dc.ci] = tolower(B[Dc.ri][Dc.ci]);
			BCP[bi] = B[Dc.ri][Dc.ci];
			bi++;
		}
	}
}

bool UseCapturedPiece(int wi,int bi,int Turn)
{
	char ch;
	int i;
	if (Turn==White && wi > 0)
	{
		cout << "Do you want to use captured Piece(y/n): ";
		cin >> ch;
		ch = toupper(ch);
		if (ch == 'Y')
			return true;
		else
			return false;
	}
	else if (Turn == Black && bi > 0)
	{
		cout << "Do you want to use captured Piece(y/n): ";
		cin >> ch;
		ch = toupper(ch);
		if (ch == 'Y')
			return true;
		else
			return false;
	}
	return false;
}

void PlaceCapturedPiece(char** &B,char*& WCP, char*& BCP, int& wi, int& bi, int Turn,Position Sc, Position Dc, int dim)
{
	char *DP = new char[20]{};
	int pi;
	int di = 0;
	if (Turn == White)
	{
		pi = Sc.ci - 11;
		if (Sc.ri == 3 || Sc.ci == 6)
		{
			pi = pi + 8;
		}
		di = 0;
		if (wi > 1)
		{
			for (int i = 0; i < wi; i++)
			{
				if (i != pi)
				{
					DP[di] = WCP[i];
					di++;
				}
			}
			delete[]WCP;
			WCP = new char[20] {};
			for (int x = 0; x < di; x++)
			{
				WCP[x] = DP[x];
			}
		}
		wi--;
	}
	else
	{
		pi = Sc.ci - 11;
		if (Sc.ri == 3 || Sc.ci == 6)
		{
			pi = pi + 8;
		}
		if (bi > 1)
		{
			for (int i = 0; i < bi; i++)
			{
				if (i != pi)
				{
					DP[di] = BCP[i];
					di++;
				}
			}
			delete[]BCP;
			BCP = new char[20] {};
			for (int x = 0; x < di; x++)
			{
				BCP[x] = DP[x];
			}
		}
		bi--;
	}
}

void drawPawn(int cr,int cc,int color)
{
	char sym = -37;
	SetClr(color);

	gotoRowCol(cr, cc);
	cout << sym;
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 1, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 1, cc - 1 + i);
		cout << sym;
	}
	gotoRowCol(cr - 2, cc);
	cout << sym;
}

void drawSilverGeneral(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int i = 0; i < 2; i++)
	{
		gotoRowCol(cr - i, cc);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr + 1, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 2, cc - 1 + i);
		cout << sym;
	}
	gotoRowCol(cr - 3, cc);
	cout << sym;
}

void drawGoldGeneral(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int i = 0; i < 2; i++)
	{
		gotoRowCol(cr - i, cc);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr + 1, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 2, cc - 1 + i);
		cout << sym;
	}
	gotoRowCol(cr - 3, cc - 1);
	cout << sym;
	gotoRowCol(cr - 3, cc + 1);
	cout << sym;
}

void drawLance(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int c = 0; c < 3; c++)
	{
		for (int r = 0; r < 3; r++)
		{
			gotoRowCol(cr - 1 + r, cc - 1 + c);
			cout << sym;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	gotoRowCol(cr - 2, cc);
	cout << sym;
}

void drawKnight(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 1 - i, cc);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 2, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		if (cc - 2 + i != cc + 1)
		{
			gotoRowCol(cr - 1, cc - 2 + i);
			cout << sym;
		}
	}
	gotoRowCol(cr, cc - 1);
	cout << sym;
}

void drawKing(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	gotoRowCol(cr, cc);
	cout << sym;
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr + 1, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 1, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr - 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i += 2)
	{
		gotoRowCol(cr - 3, cc - 2 + i);
		cout << sym;
	}
}

void drawBishop(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int r = 0; r < 3; r+=2)
	{
		for (int i = 0; i < 3; i++)
		{
			gotoRowCol(cr - r, cc - 1 + i);
			cout << sym;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 6; i++)
	{
		gotoRowCol(cr - 3 + i, cc);
		cout << sym;
	}
}

void drawRock(int cr, int cc, int color)
{
	char sym = -37;
	SetClr(color);
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr - 1, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 3; i++)
	{
		gotoRowCol(cr - 2, cc - 1 + i);
		cout << sym;
	}
	for (int i = 0; i < 5; i++)
	{
		gotoRowCol(cr + 2, cc - 2 + i);
		cout << sym;
	}
	for (int i = 0; i < 6; i++)
	{
		gotoRowCol(cr - 3 + i, cc);
		cout << sym;
	}
}

void DrawPiece(char c, int ri, int ci, int color)
{
	switch (c)
	{
	case 'L':
	case'l':
		drawLance(ri, ci, color);
		break;
	case 'S':
	case 's':
		drawSilverGeneral(ri, ci, color);
		break;
	case 'G':
	case'g':
		drawGoldGeneral(ri, ci, color);
		break;
	case 'P':
	case 'p':
		drawPawn(ri, ci, color);
		break;
	case 'B':
	case'b':
		drawBishop(ri, ci, color);
		break;
	case 'R':
	case 'r':
		drawRock(ri, ci, color);
		break;
	case 'N':
	case 'n':
		drawKnight(ri, ci, color);
		break;
	case 'K':
	case 'k':
		drawKing(ri, ci, color);
		break;
	}
}

void CaputuredPieces(char* WCP, char* BCP, int wi, int bi)
{
	gotoRowCol(9, 90);
	cout << "P1's Captured Pieces\n" << endl;
	int color;
	int Brows = 8, Bcols = 8;
	for (int ri = 0; ri < 2; ri++)
	{
		for (int ci = 0; ci < 9; ci++)
		{
			if ((ri + ci) % 2 == 0)
			{
				DrawBox(10 + (ri * Brows), 90 + (ci * Bcols), Brows, Bcols, 7);
			}
			else
			{
				DrawBox(10 + (ri * Brows), 90 + (ci * Bcols), Brows, Bcols, 8);
			}
		}
	}
	int ri = 0, ci = 0;
	for (int i = 0; i < wi; i++, ci++)
	{
		if (ci < 9)
		{
			DrawPiece(WCP[i], 10 + (ri * Brows) + Brows / 2, 90 + (ci * Bcols) + Bcols / 2, 15);
		}
		else
		{
			ci = 0;
			ri++;
			DrawPiece(WCP[i], 10 + (ri * Brows) + Brows / 2, 90 + (ci * Bcols) + Bcols / 2, 15);
		}
	}
	gotoRowCol(37, 90);
	SetClr(15);
	cout << "P2's Captured Pieces\n" << endl;
	for (int ri = 0; ri < 2; ri++)
	{
		for (int ci = 0; ci < 9; ci++)
		{
			if ((ri + ci) % 2 == 0)
			{
				DrawBox(38 + (ri * Brows), 90 + (ci * Bcols), Brows, Bcols, 7);
			}
			else
			{
				DrawBox(38 + (ri * Brows), 90 + (ci * Bcols), Brows, Bcols, 8);
			}
		}
	}
	ri = 0, ci = 0;
	for (int i = 0; i < bi; i++, ci++)
	{
		if (ci < 9)
		{
			DrawPiece(BCP[i], 38 + (ri * Brows) + Brows / 2, 90 + (ci * Bcols) + Bcols / 2, 0);
		}
		else
		{
			ci = 0;
			ri++;
			DrawPiece(BCP[i], 38 + (ri * Brows) + Brows / 2, 90 + (ci * Bcols) + Bcols / 2, 0);
		}
	}
	gotoRowCol(81, 0);
	SetClr(15);
}

void DoPromotion(char& c)
{
	system("cls");
	gotoRowCol(35, 50);
	cout << "Do you want to promote your piece";
	DrawBox(40, 48, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 48);
	cout << "     Yes ";
	DrawBox(40, 78, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 78);
	cout << "      No ";
	Position P;
	int ri, ci;
	SelectPos(P, 9);
	P.ri = P.ri * 9, P.ci = P.ci * 9;
	if (P.ci >= 48 && P.ci <= 63)
	{
		c = 'Y';
	}
	else
	{
		c = 'N';
	}
	SetClr(15);
	system("cls");
}

void DrawBoardPiece(char** B, int dim, int Brows, int Bcols, int* PPr, int* PPc, char* WCP, char* BCP, int wi, int bi)
{
	CaputuredPieces(WCP, BCP, wi, bi);
	int color;
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			if ((ri + ci) % 2 == 0)
			{
				DrawBox(ri * Brows, ci * Bcols, Brows, Bcols, 7);
				gotoRowCol((ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2);
				if (B[ri][ci] >= 'A' && B[ri][ci] <= 'Z')
				{
					color = 127;
					if (B[ri][ci] != '-')
					{
						for (int i = 0; i < 40; i++)
						{
							if ((ri != 0 || ci != 0) && (ri == PPr[i] && ci == PPc[i]))
							{
								color = 116;
							}
						}
						DrawPiece(B[ri][ci], (ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2, color);
					}
				}
				else
				{
					color = 112;
					if (B[ri][ci] != '-')
					{
						for (int i = 0; i < 40; i++)
						{
							if ((ri != 0 || ci != 0) && (ri == PPr[i] && ci == PPc[i]))
							{
								color = 115;
							}
						}
						DrawPiece(B[ri][ci], (ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2, color);
					}
				}
			}
			else
			{
				DrawBox(ri * Brows, ci * Bcols, Brows, Bcols, 8);
				gotoRowCol((ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2);
				if (B[ri][ci] >= 'A' && B[ri][ci] <= 'Z')
				{
					color = 143;
					if (B[ri][ci] != '-')
					{
						for (int i = 0; i < 40; i++)
						{
							if ((ri != 0 || ci != 0) && (ri == PPr[i] && ci == PPc[i]))
							{
								color = 132;
							}
						}
						DrawPiece(B[ri][ci], (ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2, color);
					}
				}
				else
				{
					color = 128;
					if (B[ri][ci] != '-')
					{
						for (int i = 0; i < 40; i++)
						{
							if ((ri != 0 || ci != 0) && (ri == PPr[i] && ci == PPc[i]))
							{
								color = 131;
							}
						}
						DrawPiece(B[ri][ci], (ri * Brows) + Brows / 2, (ci * Bcols) + Bcols / 2, color);
					}
				}
			}
		}
	}
	DrawBox(75, 145, 10, 20, 4);
	gotoRowCol(80, 153);
	SetClr(75);
	cout << "UNDO";
	gotoRowCol(Brows * dim, 0);
	SetClr(15);
}

void DrawPieceMove(char** B, int dim,Position Sc,Position Dc, int Brows, int Bcols, int* PPr, int* PPc, char* WCP, char* BCP, int wi, int bi)
{
	CaputuredPieces(WCP, BCP, wi, bi);
	int color;
	if (Sc.ri >= 0 && Sc.ri <= dim && Sc.ci >= 0 && Sc.ci <= dim)
	{
		if ((Sc.ri + Sc.ci) % 2 == 0)
		{
			DrawBox(Sc.ri * Brows, Sc.ci * Bcols, Brows, Bcols, 7);
		}
		else
		{
			DrawBox(Sc.ri * Brows, Sc.ci * Bcols, Brows, Bcols, 8);
		}
	}
	if ((Dc.ri + Dc.ci) % 2 == 0)
	{
		DrawBox(Dc.ri * Brows, Dc.ci * Bcols, Brows, Bcols, 7);
		gotoRowCol((Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2);
		if (B[Dc.ri][Dc.ci] >= 'A' && B[Dc.ri][Dc.ci] <= 'Z')
		{
			color = 127;
			if (B[Dc.ri][Dc.ci] != '-')
			{
				for (int i = 0; i < 40; i++)
				{
					if ((Dc.ri != 0 || Dc.ci != 0) && (Dc.ri == PPr[i] && Dc.ci == PPc[i]))
					{
						color = 116;
					}
				}
				DrawPiece(B[Dc.ri][Dc.ci], (Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2, color);
			}
		}
		else
		{
			color = 112;
			if (B[Dc.ri][Dc.ci] != '-')
			{
				for (int i = 0; i < 40; i++)
				{
					if ((Dc.ri != 0 || Dc.ci != 0) && (Dc.ri == PPr[i] && Dc.ci == PPc[i]))
					{
						color = 115;
					}
				}
				DrawPiece(B[Dc.ri][Dc.ci], (Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2, color);
			}
		}
	}
	else
	{
		DrawBox(Dc.ri * Brows, Dc.ci * Bcols, Brows, Bcols, 8);
		gotoRowCol((Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2);
		if (B[Dc.ri][Dc.ci] >= 'A' && B[Dc.ri][Dc.ci] <= 'Z')
		{
			color = 143;
			if (B[Dc.ri][Dc.ci] != '-')
			{
				for (int i = 0; i < 40; i++)
				{
					if ((Dc.ri != 0 || Dc.ci != 0) && (Dc.ri == PPr[i] && Dc.ci == PPc[i]))
					{
						color = 132;
					}
				}
				DrawPiece(B[Dc.ri][Dc.ci], (Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2, color);
			}
		}
		else
		{
			color = 128;
			if (B[Dc.ri][Dc.ci] != '-')
			{
				for (int i = 0; i < 40; i++)
				{
					if ((Dc.ri != 0 || Dc.ci != 0) && (Dc.ri == PPr[i] && Dc.ci == PPc[i]))
					{
						color = 131;
					}
				}
				DrawPiece(B[Dc.ri][Dc.ci], (Dc.ri * Brows) + Brows / 2, (Dc.ci * Bcols) + Bcols / 2, color);
			}
		}
	}
	DrawBox(75, 145, 10, 20, 4);
	gotoRowCol(80, 153);
	SetClr(75);
	cout << "UNDO";
	gotoRowCol(Brows * dim, 0);
	SetClr(15);
}

void AskPromote(char** B, int dim, Position P, int Turn, int& i, int* PPr, int* PPc, char* WCP, char* BCP, int wi, int bi)
{
	bool Promote = false;
	switch (B[P.ri][P.ci])
	{
	case 'P':
	case 'L':
		if (P.ri < 1)
		{
			Promote = true;
			PPr[i] = P.ri;
			PPc[i] = P.ci;
			i++;
			DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
	case 'p':
	case 'l':
		if (P.ri > 7)
		{
			Promote = true;
			PPr[i] = P.ri;
			PPc[i] = P.ci;
			i++;
			DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
	case 'N':
		if (P.ri < 2)
		{
			Promote = true;
			PPr[i] = P.ri;
			PPc[i] = P.ci;
			i++;
			DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
	case 'n':
		if (P.ri > 6)
		{
			Promote = true;
			PPr[i] = P.ri;
			PPc[i] = P.ci;
			i++;
			DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
	}
	if (Promote == false)
	{
		char Ans;
		if (B[P.ri][P.ci] != 'g' && B[P.ri][P.ci] != 'G' && B[P.ri][P.ci] != 'k' && B[P.ri][P.ci] != 'K')
		{
			DoPromotion(Ans);
			if (Ans == 'Y')
			{
				PPr[i] = P.ri;
				PPc[i] = P.ci;
				i++;
				DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
			}
			else
				DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
	}
}

void selectSc(Position& Sc,int dim)
{
	gotoRowCol(77, 0);
	cout << "                         ";
	gotoRowCol(77, 0);
	cout << "Source Position : ";
	SelectPos(Sc, dim);
	PrintPosition(Sc);
}

void selectDc(Position& Dc, int dim)
{
	gotoRowCol(79, 0);
	cout << "                            ";
	gotoRowCol(79, 0);
	cout << "Destination Position : ";
	SelectPos(Dc, dim);
	PrintPosition(Dc);
	for (int r = 77; r < 85; r++)
	{
		for (int c = 0; c < 30; c++)
		{
			gotoRowCol(r, c);
			cout << " ";
		}
	}
}

void init(char**& B, int& dim, int& Turn, string PNames[], bool**& PM, int*& PPr, int*& PPc, char*& WCP, char*& BCP, int& wi, int& bi, int& i)
{
	char c;
	startGame(c);
	ifstream Rdr("Game.txt");
	Rdr >> dim;
	for (int i = 0; i < 2; i++)
		Rdr >> PNames[i];
	if (c == 'S')
	{
		B = new char* [dim] {};
		for (int i = 0; i < dim; i++)
			B[i] = new char[dim] {};
		for (int ri = 0; ri < dim; ri++)
		{
			for (int ci = 0; ci < dim; ci++)
			{
				Rdr >> B[ri][ci];
			}
		}
		PPr = new int[40] {};
		PPc = new int[40] {};
		WCP = new char[20] {};
		BCP = new char[20] {};
		wi = 0;
		bi = 0;
		i = 0;
		Turn = White;
	}
	else
	{
		ifstream Read("Save.txt");
		B = new char* [dim] {};
		for (int i = 0; i < dim; i++)
			B[i] = new char[dim] {};
		for (int ri = 0; ri < dim; ri++)
		{
			for (int ci = 0; ci < dim; ci++)
			{
				Read >> B[ri][ci];
			}
		}
		PPr = new int[40] {};
		PPc = new int[40] {};
		WCP = new char[20] {};
		BCP = new char[20] {};
		Read >> wi;
		for (int x = 0; x < wi; x++)
		{
			Read >> WCP[x];
		}
		Read >> bi;
		for (int x = 0; x < bi; x++)
		{
			Read >> BCP[x];
		}
		Read >> Turn;
		Read >> i;
		for (int x = 0; x < i; x++)
		{
			Read >> PPr[x];
		}
		for (int x = 0; x < i; x++)
		{
			Read >> PPc[x];
		}
		
	}
	PM = new bool* [dim] {};
	for (int i = 0; i < dim; i++)
	{
		PM[i] = new bool[dim] {};
	}
}

void SaveBoard(char** B, int dim, int* PPr, int* PPc, char* WCP, char* BCP, int wi, int bi, int i,int Turn)
{
	ofstream writer("Save.txt");
	for (int ri = 0; ri < dim; ri++)
	{
		for (int ci = 0; ci < dim; ci++)
		{
			writer << B[ri][ci];
		}
		writer << endl;
	}
	writer << wi << endl;
	for (int x = 0; x < wi; x++)
	{
		writer << WCP[x];
	}
	writer << endl;
	writer << bi << endl;
	for (int x = 0; x < bi; x++)
	{
		writer << BCP[x];
	}
	writer << endl;
	writer << Turn << endl;
	writer << i << endl;
	for (int x = 0; x < i; x++)
	{
		writer << PPr[x] << " ";
	}
	writer << endl;
	for (int x = 0; x < i; x++)
	{
		writer << PPc[x] << " ";
	}
}

void undoInit(char***& UndoB,int dim, int**& UndoPPr, int**& UndoPPc, char**& UndoWCP, char**& UndoBCP, int*& Undowi, int*& Undobi, int*& Undoi)
{
	UndoB = new char** [1000] {};
	for (int i = 0; i < 1000; i++)
	{
		UndoB[i] = new char* [dim] {};
		for (int c = 0; c < dim; c++)
		{
			UndoB[i][c] = new char[dim] {};
		}
	}
	UndoPPr = new int* [1000] {};
	for (int i = 0; i < 1000; i++)
	{
		UndoPPr[i] = new int [40] {};
	}
	UndoPPc = new int* [1000] {};
	for (int i = 0; i < 1000; i++)
	{
		UndoPPc[i] = new int [40] {};
	}
	UndoWCP = new char* [1000] {};
	for (int i = 0; i < 1000; i++)
	{
		UndoWCP[i] = new char [20] {};
	}
	UndoBCP = new char* [1000] {};
	for (int i = 0; i < 1000; i++)
	{
		UndoBCP[i] = new char [20] {};
	}
	Undowi = new int[1000] {};
	Undobi = new int[1000] {};
	Undoi = new int[1000] {};
}

bool DoReplay()
{
	system("cls");
	gotoRowCol(35, 50);
	cout << "Do you want to replay game";
	DrawBox(40, 48, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 48);
	cout << "     Yes ";
	DrawBox(40, 78, 10, 15, 3);
	SetClr(63);
	gotoRowCol(44, 78);
	cout << "      No ";
	Position P;
	int ri, ci;
	SelectPos(P, 9);
	P.ri = P.ri * 9, P.ci = P.ci * 9;
	SetClr(15);
	system("cls");
	if (P.ci >= 48 && P.ci <= 63)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void _Sleep(int n)
{
	for (int i = 0; i < n * 10000; i++);
}

void Replay(char***& UndoB, int& UndoI, char**& B, int dim, int& Turn, int*& PPr, int*& PPc, char*& WCP, char*& BCP, int& wi, int& bi, int& i, int**& UndoPPr, int**& UndoPPc, char**& UndoWCP, char**& UndoBCP, int*& Undowi, int*& Undobi, int*& Undoi)
{
	do
	{
		Undo(UndoB, UndoI, B, dim, Turn, PPr, PPc, WCP, BCP, wi, bi, i, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
		DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		_Sleep(20000);
	} while (UndoI > 0);
}

int main()
{
	char** B;
	int dim, Turn, wi, bi, i, UndoI = 0;
	string PNames[2];
	Position Sc, Dc;
	bool** PM;
	char** DB, ** UndoWCP, ** UndoBCP;
	int* PPr, * PPc, * Undowi, * Undobi, * Undoi;
	int** UndoPPc, ** UndoPPr;
	char* WCP, * BCP;
	char*** UndoB;
	init(B, dim, Turn, PNames, PM, PPr, PPc, WCP, BCP, wi, bi, i);
	undoInit(UndoB, dim, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
	undoBoard(UndoB, UndoI, B, dim, Turn, PPr, PPc, WCP, BCP, wi, bi, i, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
	DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
	while (true)
	{
		TurnMsg(PNames[Turn]);
		do
		{
			do
			{
				do
				{
					do
					{
						selectSc(Sc, dim);
					} while (!IsValidScMain(B, Sc, dim, Turn, WCP, BCP));
					if (clickedUndo(Sc))
					{
						break;
					}
					DuplicateBoard(DB, B, dim);
					PM = ComputeHighlight(B, Sc, dim, Turn, PPr, PPc, DB,WCP,BCP);
					highlight(B, PM, dim,Sc);
					selectDc(Dc, dim);
					unhighlight(B, PM, dim,Sc);
				} while (!IsValidDc(B, Dc, dim, Turn));
				DuplicateBoard(DB, B, dim);
			} while (!isLegalMoveMain(B, Sc, Dc, PPr, PPc, Turn, WCP, BCP, DB, dim));
			if (clickedUndo(Sc))
				break;
			UpdateBoardMain(DB, Sc, Dc, WCP, BCP);
		} while (SelfCheck(DB, dim, Turn, PPr, PPc));
		if (!clickedUndo(Sc))
		{
			isCaptured(B, Dc, Turn, WCP, BCP, wi, bi, PPr, PPc);
			UpdateBoardMain(B, Sc, Dc, WCP, BCP);
			PromotedPieceMove(Sc, Dc, PPr, PPc);
			if (usingCaptured(Sc, dim))
			{
				PlaceCapturedPiece(B, WCP, BCP, wi, bi, Turn, Sc, Dc, dim);
			}
			DrawPieceMove(B, dim,Sc,Dc, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
			CaputuredPieces(WCP, BCP, wi, bi);
			if (!usingCaptured(Sc, dim) && isPromotable(Dc, Turn, PPr, PPc))
			{
				AskPromote(B, dim, Dc, Turn, i, PPr, PPc, WCP, BCP, wi, bi);
			}
			if (CheckMate(DB, B, dim, Turn, PPr, PPc))
			{
				gotoRowCol(77, 0);
				SetClr(4);
				cout << "......CheckMate......\n\n" << endl;

				cout <<"....."<< PNames[Turn] << " wins.....";
				_Sleep(20000);
				gotoRowCol(83, 0);
				SetClr(15);
				if (DoReplay())
				{
					Replay(UndoB, UndoI, B, dim, Turn, PPr, PPc, WCP, BCP, wi, bi, i, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
				}
				return 0;
			}
			if (Check(B, dim, Turn, PPr, PPc))
			{
				gotoRowCol(82, 0);
				SetClr(4);
				cout << ".....Check....." << endl;
				SetClr(15);
			}
		}
		if (clickedUndo(Sc))
		{
			Undo(UndoB, UndoI, B, dim, Turn, PPr, PPc, WCP, BCP, wi, bi, i, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
			DrawBoardPiece(B, dim, 8, 8, PPr, PPc, WCP, BCP, wi, bi);
		}
		else
		{
			ChangeTurn(Turn);
			undoBoard(UndoB, UndoI, B, dim, Turn, PPr, PPc, WCP, BCP, wi, bi, i, UndoPPr, UndoPPc, UndoWCP, UndoBCP, Undowi, Undobi, Undoi);
		}
		SaveBoard(B, dim, PPr, PPc, WCP, BCP, wi, bi, i, Turn);
	}
	return 0;
}
