#pragma once

#define SEND(s, ...) sprintf(sprintfBuffer, s, __VA_ARGS__);_Send(sprintfBuffer)
#define FORCESEND(s, ...) bool o = batchSend;batchSend = false;SEND(s, __VA_ARGS__);batchSend = o

class UsartGpu
{
private:
	bool batchSend = true;
	HardwareSerial* serial;

	char* sprintfBuffer;
	char* batchedBuffer;
	int batchedIdx;

	void _Send(const char* str)
	{
		int length = strlen(str);
		if (batchSend)
		{
			if (batchedIdx + length > 1022)
			{
				Serial.write("[UsartGpu] Batched buffer overflow!");
				return;
			}
			strncpy((char*)batchedBuffer + batchedIdx, str, length);
			batchedIdx += length;
		}
		else
		{
			serial->write((uint8_t*)str, length);
			serial->write("\r\n");
		}
	}

public:
	enum TouchScreenMode
	{
		TouchScreenMode_Disabled = 0,
		TouchScreenMode_ReturnCoord = 1,
		TouchScreenMode_ReturnHotspot = 2,
		TouchScreenMode_ReturnNothing = 3,
		TouchScreenMode_Advanced = 4,
		TouchScreenMode_5Key = 5
	};

	UsartGpu()
	{
		sprintfBuffer = new char[512];
		batchedBuffer = new char[1024];
		batchedIdx = 0;
	}
	~UsartGpu()
	{
		delete[] sprintfBuffer;
		delete[] batchedBuffer;
	}
	void SetSerial(HardwareSerial* serial)
	{
		this->serial = serial;
	}
	void SetBatchSend(bool sendNow)
	{
		this->batchSend = sendNow;
	}
	void SendBatched()
	{
		serial->write((uint8_t*)batchedBuffer, batchedIdx);
		serial->write("\r\n");
		batchedIdx = 0;
	}

	void BackgroundPicture(int backId, int x, int y, int pictureId, int hide)
	{ 
		SEND("BPIC(%d,%d,%d,%d,%d);", backId, x, y, pictureId, hide);
	}
	void Button(int hotspotId, int x1, int y1, int x2, int y2, int type, int ret)
	{
		SEND("BTN(%d,%d,%d,%d,%d,%d,%d);", hotspotId, x1, y1, x2, y2, type, ret);
	} 
	void ButtonReturn(int hotspotId, int ret)
	{
		SEND("TASC(%d,%d);", hotspotId, ret);
	}
	void ClearScreen(int colorCode = 0)
	{
		SEND("CLS(%d);", colorCode);
	}
	void Curve(int x, int y, int xpixel, int ypixel, int xn, int yn)
	{ 
		SEND("DQX(%d,%d,%d,%d,%d,%d);", x, y, xpixel, yn, xn, ypixel);
	}
	void CurveRefresh()
	{
		SEND("RQX;");
	}
	void CurveProvideDataNoRefresh(uint8_t value)
	{
		SEND("Q%d;", value);
	}
	void CurveProvideDataAndRefresh(uint8_t value)
	{
		SEND("S%d;", value);
	} 
	void DisplayInfo(int x, int y)
	{
		SEND("SNF(%d,%d);", x, y);
	}
	void GetInfo()
	{
		SEND("INF;");
	}
	void Icon(int x, int y, int pictureId, int xn, int yn, int id)
	{
		SEND("ICON(%d,%d,%d,%d,%d,%d);", x, y, pictureId, xn, yn, id);
	}
	void InputBufferDefault(const char* str)
	{
		SEND("TPIS('%s');", str);
	}
	void InputBufferDisplay(int size, int x, int y, int limitx, int lengthMax, int type, int colorCode, int lengthNeeded)
	{
		SEND("TSIN(%d,%d,%d,%d,%d,%d,%d,%d);", size, x, y, limitx, lengthMax, type, colorCode, lengthNeeded);
	}
	void Label(int size, int x1, int y1, int x2, const char* str, int colorCode, int align)
	{ 
		SEND("LABL(%d,%d,%d,%d,'%s',%d,%d);", size, x1, y1, x2, str, colorCode, align);
	}
	void LabelPicture(int size, int x1, int y1, int x2, const char* str, int colorCode, int align, int backId)
	{
		SEND("PLAB(%d,%d,%d,%d,'%s',%d,%d,%d);", size, x1, y1, x2, str, colorCode, align, backId);
	}
	void Loopback(const char* str)
	{
		SEND("SEND('%s')", str);
	}
	void MenuInitialize(int x, int y, int width, int height, int iconsize, int xicon, int xtext, int textsize, int yn, int itemcount, int menupage, int reserved = 1)
	{
		SEND("MENU(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);", x, y, width, height, iconsize, xicon, xtext, textsize, yn, itemcount, menupage, reserved);
	}
	void MenuRefresh()
	{ 
		FORCESEND("MSHW;"); 
	}
	void MenuSelectedItem(int idx)
	{
		FORCESEND("MSET(%d);", idx);
	}
	void MenuDefineItem(int idx, int iconid, const char* str)
	{
		SEND("MDF(%d,%d,'%s');", idx, iconid, str);
	} 
	void Paragraph(int size, int x1, int y1, int x2, int lineSpacing, const char* str, int colorCode)
	{
		SEND("BS%d(%d,%d,%d,%d,'%s',%d);", size, x1, y1, x2, lineSpacing, str, colorCode);
	}
	void PaintSpot(int x, int y, int colorCode)
	{
		SEND("PS(%d,%d,%d);", x, y, colorCode);
	}
	void PaintLine(int x1, int y1, int x2, int y2, int colorCode)
	{ 
		SEND("PL(%d,%d,%d,%d,%d);", x1, y1, x2, y2, colorCode);
	}
	void PaintBox(int x1, int y1, int x2, int y2, int colorCode)
	{ 
		SEND("BOX(%d,%d,%d,%d,%d);", x1, y1, x2, y2, colorCode);
	}
	void PaintBoxFilled(int x1, int y1, int x2, int y2, int colorCode)
	{
		SEND("BOXF(%d,%d,%d,%d,%d);", x1, y1, x2, y2, colorCode);
	}
	void PaintBoxBold(int x1, int y1, int x2, int y2, int width, int colorCode)
	{
		SEND("DBOX(%d,%d,%d,%d,%d,%d);", x1, y1, x2, y2, width, colorCode);
	}
	void PaintBoxBackground(int backId, int x1, int y1, int x2, int y2, int width)
	{
		SEND("PBOX(%d,%d,%d,%d,%d,%d);", backId, x1, y1, x2, y2, width);
	}
	void PaintCircle(int x, int y, int rad, int colorCode)
	{ 
		SEND("CIR(%d,%d,%d,%d);", x, y, rad, colorCode);
	}
	void PaintCircleFilled(int x, int y, int rad, int colorCode)
	{
		SEND("CIRF(%d,%d,%d,%d);", x, y, rad, colorCode);
	}
	void PaintBoxCircleCorner(int x1, int y1, int x2, int y2, int rad, int colorCode)
	{
		SEND("CBOX(%d,%d,%d,%d,%d,%d);", x1, y1, x2, y2, rad, colorCode);
	}	
	void PaintBoxCircleCornerFilled(int x1, int y1, int x2, int y2, int rad, int colorCode)
	{
		SEND("CBOF(%d,%d,%d,%d,%d,%d);", x1, y1, x2, y2, rad, colorCode);
	}
	void Picture(int x, int y, int pictureId)
	{
		SEND("PIC(%d,%d,%d);", x, y, pictureId);
	}
	void PictureCut(int x, int y, int pictureId, int px, int py, int width, int height)
	{
		SEND("CPIC(%d,%d,%d,%d,%d,%d,%d);", x, y, pictureId, px, py, width, height);
	}
	void PictureString(int size, int backId, int x, int y, const char* str, int colorCode, int limitx = -1)
	{
		if (limitx == -1)
		{
			SEND("PS%d(%d,%d,%d,'%s',%d);", size, backId, x, y, str, colorCode);
		}
		else
		{
			SEND("PS%d(%d,%d,%d,'%s',%d,%d);", size, backId, x, y, str, colorCode, limitx);
		}
	}
	void PictureButton(int hotspotId, int x1, int y1, int iconIdUp, int iconIdDown, int ret)
	{
		SEND("BICN(%d,%d,%d,%d,%d,%d);", hotspotId, x1, y1, iconIdUp, iconIdDown, ret);
	}
	void PictureButtonSetup(int pictureId, int xn, int yn)
	{
		SEND("TICN(%d,%d,%d);", pictureId, xn, yn);
	}
	void SetColorCode(int colorCode, uint16_t color)
	{
		SEND("SCC(%d,%d);", colorCode, color);
	}
	void SetDirection(int direction)
	{
		SEND("DR%d;");
	}
	void ShowPage(int pageId)
	{
		SEND("SPG(%d);");
	}
	void SetBaseCoordinate(int x, int y)
	{
		SEND("SXY(%d,%d);", x, y);
	}
	void SetBackgroundLight(int light)
	{
		SEND("SEBL(%d);", light);
	}
	void String(int size, int x, int y, const char* str, int colorCode, int limitx = -1)
	{
		if (limitx == -1)
		{
			SEND("DS%d(%d,%d,'%s',%d);", size, x, y, str, colorCode);
		}
		else
		{
			SEND("DS%d(%d,%d,'%s',%d,%d);", size, x, y, str, colorCode, limitx);
		}
	}
	void SwitchPictureButton(int hotspot, int x, int y, int width, int height, int backIdUp, int backIdDown, int ret)
	{
		SEND("PBTN(%d,%d,%d,%d,%d,%d,%d,%d);", hotspot, x, y, width, height, backIdUp, backIdDown, ret);
	}
	void Table(int x, int y, int cellwidth, int cellheight, int columns, int rows, int colorCode)
	{
		SEND("TABL(%d,%d,%d,%d,%d,%d,%d);", x, y, cellwidth, cellheight, columns, rows, colorCode);
	}
	void TableSetCell(int size, int row, int col, const char* str, int colorCode, int backgroundColorCode, int align)
	{
		SEND("CELS(%d,%d,%d,'%s',%d,%d,%d);", size, row, col, str, colorCode, backgroundColorCode, align);
	}
	void TableSetCellPictureBackground(int size, int row, int col, const char* str, int colorCode, int backId, int align)
	{
		SEND("CELP(%d,%d,%d,'%s',%d,%d,%d);", size, row, col, str, colorCode, backId, align);
	}	
	void TableUseCell(int row, int col)
	{
		SEND("CEUE(%d,%d);", row, col);
	}
	void TerminalMode()
	{
		SEND("TERM;");
	}
	void TouchScreenCalibration(int width, int height)
	{
		SEND("TPST(%d,%d);", width, height);
	}
	void TouchScreenMode(int mode, int default5key = -1)
	{
		if (default5key != -1)
		{
			SEND("TPN(%d,%d);", mode, default5key);
		}
		else
		{
			SEND("TPN(%d);", mode);
		}
	}
	void TouchScreenPencil(bool open, int colorCode)
	{
		if (open)
		{
			SEND("TPOW(%d);", colorCode);
		}
		else
		{ 
			SEND("TPOW(255)");
		}
	}
	void Windows8Box(int x1, int x2, int y1, int y2, int color)
	{
		SEND("W8BF(%d,%d,%d,%d,%d);", x1, x2, y1, y2, color);
	} 
	void Windows8TileScheme(int width, int height, const char * define)
	{
		SEND("W8DF(%d,%d,'%s');", width, height, define);
	}
	void Windows8TileRender(int x, int y, int singleTileWidth, int singleTileHeight, int spacing, int mode)
	{
		SEND("W8MU(%d,%d,%d,%d,%d,%d);", x, y, singleTileWidth, singleTileHeight, spacing, mode);
	}
	void Windows8TileUse(int tileId)
	{
		SEND("W8UE(%d);", tileId);
	}
	void Windows8TileSelect(int tileId)
	{
		SEND("W8SE(%d);", tileId);
	}
	void Windows8Hotspot(int hotspotId, int	x1, int y1, int x2, int y2, int color, int ret)
	{
		SEND("W8TP(%d,%d,%d,%d,%d,%d,%d);", hotspotId, x1, x2, y1, y2, color, ret);
	}
};