#pragma once

void InitDefaultWorkerThreadSettings();
void InitScreenCaptureAndSendThread();
void ListenToNewConnectionsThread( void *arg );
void ScreenCaptureAndSendThread( void *arg );
void ShutDownNetworkThreads();