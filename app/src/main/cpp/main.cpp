#include <jni.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_hudapps_wakeuplan_MainActivity_sendMagicPacketJNI(
        JNIEnv *env,
        jobject /* this */,
        jstring macAddrJ,
        jstring broadcastJ) {
    const char* macAddrC = env->GetStringUTFChars(macAddrJ, nullptr);
    const char* broadcastC = env->GetStringUTFChars(broadcastJ, nullptr);

    unsigned char packet[102];
    std::fill(packet, packet + 6, 0xFF);

    unsigned char mac[6];
    std::string macAddr(macAddrC);
    std::replace(macAddr.begin(), macAddr.end(), ':', '-');
    if (sscanf(macAddr.c_str(), "%hhx-%hhx-%hhx-%hhx-%hhx-%hhx",
               &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6) {
        env->ReleaseStringUTFChars(macAddrJ, macAddrC);
        env->ReleaseStringUTFChars(broadcastJ, broadcastC);
        return env->NewStringUTF("Invalid MAC address format.");
    }

    for (int i = 0; i < 16; ++i)
        std::copy(mac, mac + 6, packet + 6 + i * 6);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        env->ReleaseStringUTFChars(macAddrJ, macAddrC);
        env->ReleaseStringUTFChars(broadcastJ, broadcastC);
        return env->NewStringUTF("Failed to create socket.");
    }

    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        close(sock);
        env->ReleaseStringUTFChars(macAddrJ, macAddrC);
        env->ReleaseStringUTFChars(broadcastJ, broadcastC);
        return env->NewStringUTF("Failed to set socket to broadcast mode.");
    }

    struct sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(7);
    if (inet_pton(AF_INET, broadcastC, &destAddr.sin_addr) != 1) {
        close(sock);
        env->ReleaseStringUTFChars(macAddrJ, macAddrC);
        env->ReleaseStringUTFChars(broadcastJ, broadcastC);
        return env->NewStringUTF("Invalid broadcast address format.");
    }

    int sent = sendto(sock, packet, sizeof(packet), 0,
                      (struct sockaddr*)&destAddr, sizeof(destAddr));
    close(sock);
    env->ReleaseStringUTFChars(macAddrJ, macAddrC);
    env->ReleaseStringUTFChars(broadcastJ, broadcastC);

    if (sent < 0) {
        return env->NewStringUTF("Failed to send magic packet.");
    }
    return env->NewStringUTF("Magic packet sent successfully.");
}