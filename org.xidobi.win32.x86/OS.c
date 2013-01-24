/*
 * Copyright 2013 Gemtec GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <windows.h>

#include "jni.h"

#include "OS_structs.h"
#include "OS.h"

/*
 * Sets the value of GetLastError() to the given jobject.
 */
void setLastNativeError(JNIEnv *env, jobject lastError) {
	DWORD err = GetLastError();
	setINT(env, lastError, &err);
}

/*
 * Class:     org_xidobi_OS
 * Method:    CreateFile
 * Signature: (Ljava/lang/String;IIIIIILorg/xidobi/structs/INT;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_CreateFile(JNIEnv *env, jobject this,
		jstring lpFileName,
		jint dwDesiredAccess,
		jint dwShareMode,
		jint lpSecurityAttributes,
		jint dwCreationDisposition,
		jint dwFlagsAndAttributes,
		jint hTemplateFile,
		jobject lastError) {

	const char* fileName = (*env)->GetStringUTFChars(env, lpFileName, NULL);

	HANDLE handle = CreateFileA(fileName,
								dwDesiredAccess,
								dwShareMode,
								(LPSECURITY_ATTRIBUTES) lpSecurityAttributes,
								dwCreationDisposition,
								dwFlagsAndAttributes,
								(HANDLE) hTemplateFile);

	setLastNativeError(env, lastError);

	(*env)->ReleaseStringUTFChars(env, lpFileName, fileName);

	return (jint) handle;
}

/*
 * Class:     org_xidobi_OS
 * Method:    CloseHandle
 * Signature: (ILorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_CloseHandle(JNIEnv *env, jobject this,
		jint handle,
		jobject lastError) {

	BOOL result = CloseHandle((HANDLE) handle);

	setLastNativeError(env, lastError);

	if (!result)
		return JNI_FALSE;
	return JNI_TRUE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    GetCommState
 * Signature: (ILorg/xidobi/structs/DCB;Lorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_GetCommState(JNIEnv *env, jobject this,
		jint handle,
		jobject dcbObject,
		jobject lastError) {
	DCB dcb;
	FillMemory(&dcb, sizeof(dcb), 0);

	BOOL result = GetCommState((HANDLE) handle, &dcb);

	setLastNativeError(env, lastError);

	if (!result)
		return JNI_FALSE;

	setDCBFields(env, dcbObject, &dcb);

	return JNI_TRUE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    SetCommState
 * Signature: (ILorg/xidobi/structs/DCB;Lorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_SetCommState(JNIEnv *env, jobject this,
		jint handle,
		jobject dcbObject,
		jobject lastError) {
	DCB dcb;
	getDCBFields(env, dcbObject, &dcb);

	BOOL result = SetCommState((HANDLE) handle, &dcb);

	setLastNativeError(env, lastError);

	if (!result)
		return JNI_FALSE;
	return JNI_TRUE;
}


/*
 * Class:     org_xidobi_OS
 * Method:    CreateEventA
 * Signature: (IZZLjava/lang/String;Lorg/xidobi/structs/INT;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_CreateEventA(JNIEnv *env, jobject this,
		jint lpEventAttributes,
		jboolean bManualReset,
		jboolean bInitialState,
		jstring lpName,
		jobject lastError) {

	const char* name;
	if (lpName == NULL)
		name = NULL;
	else
		name = (*env)->GetStringUTFChars(env, lpName, NULL);

	HANDLE handle = CreateEventA(	NULL,
									bManualReset,
									bInitialState,
									name);

	setLastNativeError(env, lastError);

	if (name != NULL)
		(*env)->ReleaseStringUTFChars(env, lpName, name);

	return (jint) handle;
}

/*
 * Class:     org_xidobi_OS
 * Method:    WriteFile
 * Signature: (I[BILorg/xidobi/structs/INT;Lorg/xidobi/structs/OVERLAPPED;Lorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_WriteFile(JNIEnv *env, jobject this,
		jint handle,
		jbyteArray lpBuffer,
		jint nNumberOfBytesToWrite,
		jobject lpNumberOfBytesWritten,
		jobject lpOverlapped,
		jobject lastError) {

	DWORD bytesWritten = 0;
	OVERLAPPED *overlapped = getOVERLAPPED(env, lpOverlapped);

	jbyte* jBuffer = (*env)->GetByteArrayElements(env, lpBuffer, NULL);

	BOOL result = WriteFile( (HANDLE) handle,
							 jBuffer,
							 (DWORD) nNumberOfBytesToWrite,
							 &bytesWritten,
							 overlapped);

	setLastNativeError(env, lastError);

	setINT(env, lpNumberOfBytesWritten, &bytesWritten);
	setOVERLAPPED(env, lpOverlapped, overlapped);

	(*env)->ReleaseByteArrayElements(env, lpBuffer, jBuffer, 0);

	if (result)
		return JNI_TRUE;
	return JNI_FALSE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    ReadFile
 * Signature: (I[BILorg/xidobi/structs/INT;Lorg/xidobi/structs/OVERLAPPED;Lorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_ReadFile(JNIEnv *env, jobject this,
		jint handle,
		jbyteArray lpBuffer,
		jint nNumberOfBytesToRead,
		jobject lpNumberOfBytesRead,
		jobject lpOverlapped,
		jobject lastError) {

	DWORD bytesRead = 0;
	OVERLAPPED *overlapped = getOVERLAPPED(env, lpOverlapped);

	jsize size = (*env)->GetArrayLength(env, lpBuffer);
	const jbyte jBuffer[size];

	BOOL result = ReadFile( (HANDLE) handle,
							 &jBuffer,
							 (DWORD) nNumberOfBytesToRead,
							 &bytesRead,
							 overlapped);

	setLastNativeError(env, lastError);

	setINT(env, lpNumberOfBytesRead, &bytesRead);
	setOVERLAPPED(env, lpOverlapped, overlapped);

	(*env)->SetByteArrayRegion(env, lpBuffer, 0, bytesRead, jBuffer);

	if (result)
		return JNI_TRUE;
	return JNI_FALSE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    GetLastError
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_GetLastError(JNIEnv *env, jobject this) {
	return (jint) GetLastError();
}

/*
 * Class:     org_xidobi_OS
 * Method:    MAKELANGID
 * Signature: (SS)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_MAKELANGID(JNIEnv *env, jobject this,
		jshort usPrimaryLanguage,
		jshort usSubLanguage) {
	DWORD result = MAKELANGID(usPrimaryLanguage, usSubLanguage);
	return (jint) result;
}

/*
 * Class:     org_xidobi_OS
 * Method:    FormatMessageA
 * Signature: (ILjava/lang/Void;II[BILjava/lang/Void;Lorg/xidobi/structs/INT;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_FormatMessageA(JNIEnv *env, jobject this,
		jint dwFlags,
		jobject lpSource,
		jint dwMessageId,
		jint dwLanguageId,
		jbyteArray lpBuffer,
		jint nSize,
		jobject arguments,
		jobject lastError) {

	jsize size = (*env)->GetArrayLength(env, lpBuffer);
	const jbyte jBuffer[size];

	DWORD result = FormatMessageA((DWORD) dwFlags,
								  NULL, // ignored
								  (DWORD) dwMessageId,
								  (DWORD) dwLanguageId,
								  (LPTSTR) &jBuffer,
								  (DWORD) nSize,
								  NULL); // ignored

	setLastNativeError(env, lastError);

	(*env)->SetByteArrayRegion(env, lpBuffer, 0, size, jBuffer);

	return result;
}

/*
 * Class:     org_xidobi_OS
 * Method:    GetOverlappedResult
 * Signature: (ILorg/xidobi/structs/OVERLAPPED;Lorg/xidobi/structs/INT;ZLorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_GetOverlappedResult(JNIEnv * env, jobject this,
		  jint handle,
		  jobject lpOverlapped,
		  jobject lpNumberOfBytesTransferred,
		  jboolean bWait,
		  jobject lastError){

	DWORD written = 0;

	OVERLAPPED *overlapped = getOVERLAPPED(env, lpOverlapped);

	BOOL result = GetOverlappedResult((HANDLE) handle,
									  overlapped,
									  &written,
									  (BOOL) bWait);

	setLastNativeError(env, lastError);

	setINT(env, lpNumberOfBytesTransferred, &written);
	setOVERLAPPED(env, lpOverlapped, overlapped);

	if (result)
		return JNI_TRUE;
	return JNI_FALSE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    WaitForSingleObject
 * Signature: (IILorg/xidobi/structs/INT;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_WaitForSingleObject(JNIEnv *env, jobject this,
		jint hhandle,
		jint dwMilliseconds,
		jobject lastError) {
	DWORD error = WaitForSingleObject(	(HANDLE) hhandle,
										dwMilliseconds);

	setLastNativeError(env, lastError);

	return (jint) error;
}

/*
 * Class:     org_xidobi_OS
 * Method:    RegOpenKeyExA
 * Signature: (ILjava/lang/String;IILorg/xidobi/structs/HKEY;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_RegOpenKeyExA(JNIEnv *env, jobject this,
		jint hkey,
		jstring lpSubKey,
		jint ulOptions,
		jint samDesired,
		jobject phkResult) {

	HKEY *hkresult = getHKEY(env, phkResult);

	// Casting jstring to char*.
	const char *subKey = (*env)->GetStringUTFChars(env, lpSubKey, NULL);

	LONG result = RegOpenKeyExA((HKEY) hkey,
								(LPCSTR) subKey,
								(DWORD) ulOptions,
								(REGSAM) samDesired,
								(PHKEY) hkresult);

	(*env)->ReleaseStringUTFChars(env, lpSubKey, subKey);
	setHKEY(env, phkResult, hkresult);

	return (jint) result;
}

/*
 * Class:     org_xidobi_OS
 * Method:    RegCloseKey
 * Signature: (Lorg/xidobi/structs/HKEY;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_RegCloseKey(JNIEnv *env, jobject this,
		jobject hKey) {

	HKEY *phkey = getHKEY(env, hKey);

	LONG result = RegCloseKey((HKEY) *phkey);

	return (jint) result;
}

/*
 * Class:     org_xidobi_OS
 * Method:    RegEnumValueA
 * Signature: (Lorg/xidobi/structs/HKEY;I[BLorg/xidobi/structs/INT;ILorg/xidobi/structs/INT;[BLorg/xidobi/structs/INT;)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_RegEnumValueA(JNIEnv *env, jobject this,
		jobject hKey,
		jint dwIndex,
		jbyteArray lpValueName,
		jobject lpcchValueName,
		jint lpReserved,
		jobject lpType,
		jbyteArray lpData,
		jobject lpcbData) {

	HKEY *phkey = getHKEY(env, hKey);

	// Initialize array for lpValueName.
	jsize vnSize = (*env)->GetArrayLength(env, lpValueName);
	const jbyte pValueName[vnSize];

	DWORD pcchValueName = 0;
	getINT(env, lpcchValueName, &pcchValueName);
	DWORD pType = 0;

	// Initialize array for lpData.
	jsize size = (*env)->GetArrayLength(env, lpData);
	const jbyte jBuffer[size];

	DWORD pcbData;
	getINT(env, lpcbData, &pcbData);

	LONG result = RegEnumValueA((HKEY) *phkey,
								(DWORD) dwIndex,
								(LPTSTR) pValueName,
								(LPDWORD) &pcchValueName,
								NULL,
								(LPDWORD) &pType,
								(LPBYTE) &jBuffer,
								(LPDWORD) &pcbData);

	// Copying byte arrays to lpData and lpValueName:
	(*env)->SetByteArrayRegion(env, lpData, 0, size, jBuffer);
	(*env)->SetByteArrayRegion(env, lpValueName, 0, pcchValueName, pValueName);

	setINT(env, lpcchValueName, &pcchValueName);
	setINT(env, lpType, &pType);
	setINT(env, lpcbData, &pcbData);

	return (jint) result;
}

/*
 * Class:     org_xidobi_OS
 * Method:    SetCommMask
 * Signature: (IILorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_SetCommMask(JNIEnv *env, jobject this,
		jint hFile,
		jint dwEvtMask,
		jobject lastError) {

	// TODO Implement call to SetCommMask

	return JNI_FALSE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    WaitCommEvent
 * Signature: (ILorg/xidobi/structs/INT;Lorg/xidobi/structs/OVERLAPPED;Lorg/xidobi/structs/INT;)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_xidobi_OS_WaitCommEvent(JNIEnv *env, jobject this,
		jint hFile,
		jobject lpEvtMask,
		jobject lpOverlapped,
		jobject lastError) {

	// TODO Implement call to WaitCommEvent

	return JNI_FALSE;
}

/*
 * Class:     org_xidobi_OS
 * Method:    malloc
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_malloc(JNIEnv *env, jobject this,
		jint size) {
	return (jint) malloc(size);
}

/*
 * Class:     org_xidobi_OS
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_xidobi_OS_free(JNIEnv *env, jobject this,
		jint pointer) {
	free((void *) pointer);
}

/*
 * Class:     org_xidobi_OS
 * Method:    sizeOf_OVERLAPPED
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_sizeOf_1OVERLAPPED(JNIEnv *env, jobject this) {
	return (jint) sizeof(OVERLAPPED);
}

/*
 * Class:     org_xidobi_OS
 * Method:    sizeOf_HKEY
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_xidobi_OS_sizeOf_1HKEY(JNIEnv *env, jobject this) {
	return (jint) sizeof(HKEY);
}

/*
 * Class:     org_xidobi_OS
 * Method:    getByteArray
 * Signature: (Lorg/xidobi/structs/NativeByteArray;I)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_org_xidobi_OS_getByteArray(JNIEnv *env, jobject this,
		jobject nativeByteArray,
		jint length) {

	jbyteArray result = (*env)->NewByteArray(env, length);
	jbyte *bytes = getNativeByteArray(env, nativeByteArray);
	(*env)->SetByteArrayRegion(env, result, 0, length, bytes);

	return result;
}
