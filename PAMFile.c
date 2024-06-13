#include <security/pam_modules.h>
#include <studio.h>
#include <string.h>
#include <curl/curl.h>



PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) 
{
	return PAM_SUCCESS;
}


PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) 
{

	int rval;
	const char* username;


	struct pam_response *resp;
	struct pam_conv *conv;

	rval = pam_get_user(pamh, &username, NULL);

	if (rval != PAM_SUCCESS) {
		return rval;
	}

	if (strcmp("root",username) == 0) {

		curl -X POST -d "Body=Hi there, this is a test message from cURL" -d "From=+14155238886" -d "To=+5519981418084" "https://api.twilio.com/2010-04-01/Accounts/ACdd405d71e1288878b447d34931edde44/Messages" -u "ACdd405d71e1288878b447d34931edde44:933090506f27146da5e72cabe36121c0"

		return PAM_SUCCESS;

	}

	system("pause");

	return PAM_AUTH_ERR;
}