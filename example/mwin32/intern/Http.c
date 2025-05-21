#include "MEM_guardedalloc.h"

#include "LIB_string.h"

#include "Http.h"
#include "Log.h"

#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

extern const int datatoc_env_txt_size;
extern const char datatoc_env_txt[];

#define MAXMSG 0xffffu

void ConvertGreekCodePageToUTF8(char *szText, size_t maxlen) {
	DWORD dwMultiLength = MultiByteToWideChar(1253, 0, szText, -1, NULL, 0) + 1;
	wchar_t *mbText = (wchar_t *)(MEM_mallocN(sizeof(wchar_t) * dwMultiLength, "mbText"));
	MultiByteToWideChar(1253, 0, szText, -1, mbText, dwMultiLength);
	WideCharToMultiByte(CP_UTF8, 0, mbText, -1, szText, maxlen, NULL, NULL);
	MEM_freeN(mbText);
}

void ConvertUTF8ToGreekCodePage(char *szText, size_t maxlen) {
	DWORD dwMultiLength = MultiByteToWideChar(CP_UTF8, 0, szText, -1, NULL, 0) + 1;
	wchar_t *mbText = (wchar_t *)(MEM_mallocN(sizeof(wchar_t) * dwMultiLength, "mbText"));
	MultiByteToWideChar(CP_UTF8, 0, szText, -1, mbText, dwMultiLength);
	WideCharToMultiByte(1253, 0, mbText, -1, szText, maxlen, NULL, NULL);
	MEM_freeN(mbText);
}

void Http_Upload(ListBase *pHistory, ChatMessage *pMessage) {
	pMessage->hSession = WinHttpOpen(L"ChatGPTClient/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!pMessage->hSession) {
		Logf("WinHttpOpen ERR");
		return;
	}
	pMessage->hConnect = WinHttpConnect(pMessage->hSession, L"api.openai.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (!pMessage->hConnect) {
		Logf("WinHttpConnect ERR");
		WinHttpCloseHandle(pMessage->hSession);
		return;
	}
	pMessage->hRequest = WinHttpOpenRequest(pMessage->hConnect, L"POST", L"/v1/chat/completions", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (!pMessage->hRequest) {
		Logf("WinHttpOpenRequest ERR");
		WinHttpCloseHandle(pMessage->hConnect);
		WinHttpCloseHandle(pMessage->hSession);
		return;
	}

	char *szReq = (char *)MEM_callocN(MAXMSG, "Http_Upload::Body");
	LIB_strcat(szReq, MAXMSG, "{\"model\":\"gpt-4o\",\"messages\":[{\"role\":\"system\",\"content\":\"Act like a real Metin2 player, slightly rude if needed. Respond briefly to Greek/English chat. Never admit you're a bot. If asked about items or prices, deflect saying a variation of, tha sou pw meta, or den 3erw twra, or similar. If insulted, respond coldly with .... Use the word, null, to ignore messages. Keep replies short, blunt, casual. Don't lose potential customers, but avoid long convos. Never reveal you're automated or not playing actively.\"}");
	LISTBASE_FOREACH(ChatMessage *, pOld, pHistory) {
		if (!ELEM(pOld->iState, MESSAGE_IN, MESSAGE_OK)) {
			continue;
		}

		char szText[512];
		LIB_strcpy(szText, ARRAY_SIZE(szText), pOld->szText);
		ConvertGreekCodePageToUTF8(szText, ARRAY_SIZE(szText));

		LIB_strcat(szReq, MAXMSG, ",{\"role\": ");
		if (pOld->iState == MESSAGE_IN) {
			LIB_strcat(szReq, MAXMSG, "\"user\"");
		}
		else {
			LIB_strcat(szReq, MAXMSG, "\"assistant\"");
		}
		LIB_strcat(szReq, MAXMSG, ", \"content\": \"");
		LIB_strcat(szReq, MAXMSG, szText);
		LIB_strcat(szReq, MAXMSG, "\"}");
	}
	LIB_strcat(szReq, MAXMSG, "]}");
	DWORD dwLength = LIB_strlen(szReq);

	wchar_t *szHeader = (wchar_t *)MEM_callocN(0xfff, "Http_Upload::Header");
	memcpy(POINTER_OFFSET(szHeader, 0), L"Content-Type: application/json\r\nAuthorization: Bearer ", 108);
	for (int iIndex = 0; iIndex < datatoc_env_txt_size; iIndex++) {
		((wchar_t *)POINTER_OFFSET(szHeader, 108))[iIndex] = datatoc_env_txt[iIndex];
	}

	WinHttpSendRequest(pMessage->hRequest, szHeader, 54 + datatoc_env_txt_size, (LPVOID)szReq, dwLength, dwLength, pMessage);

	MEM_freeN(szReq);
}

void Http_Poll(ChatMessage *pMessage) {
	WinHttpReceiveResponse(pMessage->hRequest, NULL);

	char *szRes = (char *)MEM_callocN(MAXMSG, "Http_Upload::Body");

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;

	do {
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(pMessage->hRequest, &dwSize))
			break;
		pszOutBuffer = (char *)MEM_mallocN(dwSize + 1, "Buffer");
		if (!pszOutBuffer) {
			break;
		}
		ZeroMemory(pszOutBuffer, dwSize + 1);
		if (WinHttpReadData(pMessage->hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
			LIB_strncat(szRes, MAXMSG, pszOutBuffer, dwDownloaded);
		}
		MEM_freeN(pszOutBuffer);
	} while (dwSize > 0);

	pMessage->iState = MESSAGE_ERR;

	const char *szItr = szRes;
	if (szItr = LIB_strfind(szItr, szRes + MAXMSG, "\"choices\"")) {
		if (szItr = LIB_strfind(szItr, szRes + MAXMSG, "\"message\"")) {
			if (szItr = LIB_strfind(szItr, szRes + MAXMSG, "\"content\"")) {
				const char *szBegin = LIB_strnext(szItr, szRes + MAXMSG, szItr + 10, '\"') + 1;
				const char *szEnd = LIB_strnext(szItr, szRes + MAXMSG, szBegin, '\"');

				LIB_strncpy(pMessage->szText, ARRAY_SIZE(pMessage->szText), szBegin, szEnd - szBegin);
				ConvertUTF8ToGreekCodePage(pMessage->szText, ARRAY_SIZE(pMessage->szText));
				Logf("CHATGPT ANSWER : %s", pMessage->szText);
				pMessage->iState = MESSAGE_SND;
			}
		}
	}

	MEM_freeN(szRes);
}

void Http_Complete(ChatMessage *pMessage) {
	WinHttpCloseHandle(pMessage->hRequest);
	WinHttpCloseHandle(pMessage->hConnect);
	WinHttpCloseHandle(pMessage->hSession);
}
