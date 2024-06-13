#include <security/pam_modules.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define maxMessageSize = 10000

typedef enum { false, true }    bool;

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	
    return PAM_SUCCESS;

}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {

    char phone[20], phoneData[20];

    strcpy(phone, phoneData);

	int rval;
	const char *username;

	struct pam_response *resp;
	struct pam_conv *conv;

	rval = pam_get_user(pamh, &username, NULL);

	if (rval != PAM_SUCCESS) {

		return rval;

	}

	if (strcmp("root",username) == 0) {

        printf("\nAntes de realizar o login, é preciso fazer uma verificação");

        printf("\n\nInsira o seu codigo nacional: ");
        gets(phoneData);

        strcat(phone, phoneData);

        printf("\nInsira o seu DDD: ");
        gets(phoneData);

        strcat(phone, phoneData);

        printf("\nInsira o seu numero de telefone: ");
        gets(phoneData);

        strcat(phone, phoneData);

        sendMessage("ACdd405d71e1288878b447d34931edde44", "e58595ef4015069f21fe69f054b64a65", "Heelou mi freind", "+19526495464", phone, NULL, false);

		return PAM_SUCCESS;

	}

	return PAM_AUTH_ERR;
}

size_t _twilio_null_write(char *ptr, size_t size, size_t nmemb, void *userdata) {

    return size * nmemb;

}

int sendMessage(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, bool verbose) {

        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        char url[maxMessageSize];

        snprintf(url, sizeof(url), "%s%s%s",
                 "https://api.twilio.com/2010-04-01/Accounts/",
                 account_sid,
                 "/Messages");

        char parameters[maxMessageSize];

        snprintf(parameters, sizeof(parameters), "%s%s%s%s%s%s",
                    "To=",
                    to_number,
                    "&From=",
                    from_number,
                    "&Body=",
                    message);



        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parameters);
        curl_easy_setopt(curl, CURLOPT_USERNAME, account_sid);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, auth_token);

        if (!verbose) {

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _twilio_null_write);
       
        }

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        long http_code = 0;
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (res != CURLE_OK) {

                if (verbose) {

                    fprintf(stderr, "SMS send failed: %s.\n", curl_easy_strerror(res));
                
                }

                return -1;

        } else if (http_code != 200 && http_code != 201) {

                if (verbose) {

                    fprintf(stderr, "SMS send failed, HTTP Status Code: %ld.\n", http_code);

                }

                return -1;

        } else {

                if (verbose) {

                    fprintf(stderr, "SMS sent successfully!\n");
                
                }

                return 0;

        }

    return -1;

}