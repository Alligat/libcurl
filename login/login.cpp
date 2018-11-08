//
// Created by Ace on 2017/8/5.
//
//#include "OBOJni.h"
#include "cJSON.h"
#include <curl/curl.h>
#include<string.h>

#define RESPONSE_DATA_LEN 4096

//�������շ�����һ��buffer
typedef struct login_response_data
{
    login_response_data() {
        memset(data, 0, RESPONSE_DATA_LEN);
        data_len = 0;
    }

    char data[RESPONSE_DATA_LEN];
    int data_len;

}response_data_t;


//����ӷ��������ص����ݣ������ݿ�����arg��
size_t deal_response(void *ptr, size_t n, size_t m, void *arg)
{
    int count = m*n;

    response_data_t *response_data = (response_data_t*)arg;

    memcpy(response_data->data, ptr, count);

    response_data->data_len = count;

    return response_data->data_len;
}

//#define POSTDATA "{\"username\":\"gailun\",\"password\":\"123123\"}"
#define POSTDATA "{\"username\":\"gailun\",\"password\":\"123123\",\"driver\":\"yes\"}"

int main()
{
    //const char *username = env->GetStringUTFChars(j_username, 0);
    //const char *passwd = env->GetStringUTFChars(j_passwd, 0);
    //const char *isDriver = j_isDriver == 0?"yes":"no";

    char *post_str = NULL;
    /*char *post_str = "123123";

    CURL* curl = NULL;
    CURLcode res;
    response_data_t responseData;//ר��������Ŵӷ��������ص�����
*/
	CURL* curl = NULL;
	CURLcode res;
    response_data_t responseData;//ר��������Ŵӷ��������ص�����
    //��ʼ��curl���
    curl = curl_easy_init();
    if(curl == NULL) {
       // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: curl init error \n");
        return 1;
    }



    //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: username = %s, passwd = %s, isDriver = %s",username, passwd, isDriver);


    //��װһ������Э��
    /*

       ====������˵�Э��====
     http://ip:port/login [json_data]
    {
        username: "gailun",
        password: "123123",
        driver:   "yes"
    }
     *
     *
     * */
    //��1����װһ��json�ַ���
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "username", "ldw");
    cJSON_AddStringToObject(root, "password", "123123");
    cJSON_AddStringToObject(root, "driver", "yes");

    post_str = cJSON_Print(root);
    cJSON_Delete(root);
    root = NULL;


   // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: post_str = [%s]\n", post_str);



    //(2) ��web������ ����http���� ����post���� json�ַ���
    //1 ����curl url
    curl_easy_setopt(curl, CURLOPT_URL, "http://172.16.1.96:7777/login");

    //�ͻ��˺���CA֤����֤
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

    //2 ����post���󿪹�
    curl_easy_setopt(curl, CURLOPT_POST, true);
    //3 ���post����
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str);

    //4 �趨һ�������������Ӧ�Ļص�����
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, deal_response);

    //5 ���ص���������һ���β�
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    //6 ���������������,�ȴ�����������Ӧ
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:perform ERROR, rescode= [%d]\n",
                            //res);
        return 1;

    }
    curl_easy_cleanup(curl);
    //��3��  �����������Ӧ������ �˿̵�responseData���Ǵӷ�������ȡ������


    /*

      //�ɹ�
    {
        result: "ok",
    }
    //ʧ��
    {
        result: "error",
        reason: "why...."
    }

     *
     * */
    //(4) �������������ص�json�ַ���
    //cJSON *root;
    root = cJSON_Parse(responseData.data);

    cJSON *result = cJSON_GetObjectItem(root, "result");
    if(result && strcmp(result->valuestring, "ok") == 0) {
	    printf("data:%s\n",responseData.data);
        //��½�ɹ�
        //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login succ������");
        return 0;

    }
    else {
        //��½ʧ��
        cJSON* reason = cJSON_GetObjectItem(root, "reason");
        if (reason) {
            //��֪����
            //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login error, reason = %s������", reason->valuestring);

        }
        else {
            //δ֪�Ĵ���
           // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login error, reason = Unknow������");

        }

        return 1;
    }



    return 0;
}

