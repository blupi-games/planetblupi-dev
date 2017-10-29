#include <jni.h>
#include <string>

extern "C"
jstring
Java_pl_krzysh_myapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
