#ifndef CNET_STREAM_HH
#define CNET_STREAM_HH

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/** \name Detours
 * \{ */

typedef bool (__thiscall *fnCNetworkStream_Recv)(void *me, int size, char *pDestBuf);
typedef bool (__thiscall *fnCNetworkStream_Peek)(void *me, int size, char *pDestBuf);
typedef bool (__thiscall *fnCNetworkStream_Send)(void *me, int size, const char *pBuf);
extern fnCNetworkStream_Recv CNetworkStream_Recv;
extern fnCNetworkStream_Peek CNetworkStream_Peek;
extern fnCNetworkStream_Send CNetworkStream_Send;

/** \} */

void CNetStream_Attach(void *pLocation, size_t dwLength);
void CNetStream_Detach();

#ifdef __cplusplus
}
#endif

#endif	// CNET_STREAM_HH
