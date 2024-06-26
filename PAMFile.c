#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_MESSAGE_SIZE 10000
#define MAX_QUESTIONS 5

typedef enum { false, true }    bool;

typedef struct {

	char question[1000];
	char options[4][64];
	int correctOption;

} Question;

char *userPhone;
char phoneNumber[20];
int score = 0, n = 0;

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	
    return PAM_SUCCESS;

}

int isFirstAccess(const char *username);
int startGame(void);
int getUserNumber(void);
int sendMessage(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, bool verbose);
int saveUserAccess(const char *username);
int setUseTime(const char *username);

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {

    char textMessage[100];
    int rval, authCode, userCode;

	const char *username;

	struct pam_response *resp;
	struct pam_conv *conv;

	rval = pam_get_user(pamh, &username, NULL);

	if (rval != PAM_SUCCESS) {

		return rval;

	}

    srand(time(NULL));

    isFirstAccess(username);

    if(n != 1) {

        startGame();
        getUserNumber();

        authCode = rand()%(100000 - 999999) + 100000;

        snprintf(textMessage, 100, "O codigo eh: %i", authCode);

        printf("\nUm SMS foi enviado para confirmar seu login");

        saveUserAccess(username);
        sendMessage("ACdd405d71e1288878b447d34931edde44", "00e1019d3ec6acf522e56bc58cbf9f3f", textMessage, "+19526495464", phoneNumber, false);
        setUseTime(username);

    } else {

        authCode = rand()%(100000 - 999999) + 100000;

        snprintf(textMessage, 100, "O codigo eh: %i", authCode);

        printf("\n\nUm SMS foi enviado para confirmar seu login");

        sendMessage("ACdd405d71e1288878b447d34931edde44", "00e1019d3ec6acf522e56bc58cbf9f3f", textMessage, "+19526495464", phoneNumber, false);

    }

    printf("\n\n\nDigite o codigo recebido: ");
    scanf("%i", &userCode);

    if(userCode == authCode) {

        return PAM_SUCCESS;

    } else {

        return PAM_AUTH_ERR;

    }

	return PAM_AUTH_ERR;
}

int isFirstAccess(const char *username) {

    FILE *file;

    char fileLine[1000], userName [1000];

    strcpy(userName, username);

    file = fopen("/etc/pam.d/userAccesses", "r"); 

    while(fgets(fileLine, 1000, file)) {

        if(strstr(fileLine, userName)) {

            userPhone = strstr(fileLine, userName);
            userPhone = strtok(userPhone, " ");
            userPhone = strtok(NULL, " ");

            n = 1;
            strcpy(phoneNumber, userPhone);

            fclose(file);

            break;

        }
    }

    return 0;

}

void displayQuestion(Question q);
int checkAnswer(Question q, int userAnswer);

int startGame(void) {

	Question originalQuestions[MAX_QUESTIONS] = {

		{ "Ronald Green acaba de ser sequestrado. Sherlock Holmes foi intimado para resolver o caso. Ele encontrou, na cena do crime, um bilhete escrito pela vitima. O papel dizia: Dois de marco, tres de janeiro, primeiro de agosto. Sherlock sabia que o nome do sequestrador estava oculto no bilhete. Os suspeitos eram: Ana Green, filha de Ronald. John Jacobson, um funcionario, June Green, esposa e Caitlyn Chara, uma funcionaria. Quem eh o sequestrador?",
		{ "A sequestradora eh June Green", "A sequestradora eh Ana Green", "O sequestrador eh John Jacobson", "A sequestradora eh Caitlyn Chara" },
		2 },

		{ "Uma noite, Sherlock Holmes estava em casa. De repente, uma bola de neve bateu contra sua janela, quebrando o vidro. Ele se levantou e viu quatro meninos da vizinhanca, todos irmaos, correndo numa esquina. Os meninos se chamavam John Crimson, Mark Crimson, Paul Crimson e Rakan Crimson. Apos, Holmes recebeu um bilhete que dizia: ? Crimson. Ele quebrou sua janela. Qual dos quatro irmaos quebrou a janela?",
		{ "John Crimson", "Mark Crimson", "Paul Crimson", "Rakan Crimson" },
		2 },

		{ "Imagine que voce esta em uma sala escura ao lado de Sherlock. Nela ha um fosforo, uma lampada de querosene, uma vela e uma lareira. O que voce acenderia primeiro?",
		{ "O fosforo", "A lampada de querosene",
			"A vela", "A lareira" },
		1 },

		{ "Alguns meses tem 31 dias, outros tem 30 dias. Quantos meses tem 28 dias?",
		{ "1 mes", "12 meses", "6 meses", "3 meses" },
		2 },

		{ "Cinco irmas estao reunidas em um quarto. Maria esta fazendo trico, Fernanda esta desenhando, Luiza esta jogando xadrez e Lucia esta dormindo. Com quem a quinta irma esta?",
		{ "Maria", "Luiza", "Fernanda", "Lucia" },
		2 }

	};

	Question questions[MAX_QUESTIONS];
	memcpy(questions, originalQuestions, sizeof(originalQuestions));

	int numQuestions = MAX_QUESTIONS;

    system("clear");

	printf("Bem-vindo ao Quiz! Atencao, a cada enigima respondido errado haverá uma penalidade\n");

	for (int i = 0; i < MAX_QUESTIONS; i++) {

		int randomIndex = rand() % numQuestions;
		Question currentQuestion = questions[randomIndex];

		displayQuestion(currentQuestion);

		int userAnswer;

		printf("\nColoque sua resposta entre (1-4): ");
		scanf("%i", &userAnswer);

		if (userAnswer >= 1 && userAnswer <= 4) {

			if (checkAnswer(currentQuestion, userAnswer)) {

				printf("\nCorreto!\n");
				score++;

			} else {

				printf("\nIncorreto. A resposta correta eh: %i. %s\n", currentQuestion.correctOption, currentQuestion.options[currentQuestion.correctOption- 1]);
			
            }
            
		} else {

			printf("Escolha invalida. Escolha um numero entre 1 e 4.\n");
		
        }

		questions[randomIndex] = questions[numQuestions - 1];
		numQuestions--;

	}

	printf("\n\nParabens!!!! Quiz completo! Sua pontuacao foi de: %i/%i\n", score, MAX_QUESTIONS);

    return 0;

}

void displayQuestion(Question q) {

	printf("\n%s\n", q.question);

	for (int i = 0; i < 4; i++) {

		printf("%i. %s\n", i + 1, q.options[i]);

	}

}

int checkAnswer(Question q, int userAnswer) {

	return (userAnswer == q.correctOption);

}

int getUserNumber() {

    char phoneBuffer[20];

    strcpy(phoneNumber, "");

    fgets(phoneBuffer, 20, stdin);

    printf("\n\nAntes de realizar o login, eh preciso fazer uma verificacao");

    printf("\n\nInsira o seu codigo nacional: ");
    fgets(phoneBuffer, 20, stdin);

    phoneBuffer[strcspn(phoneBuffer, "\n")] = 0;
    strcat(phoneNumber, phoneBuffer);

    printf("\nInsira o seu DDD: ");
    fgets(phoneBuffer, 20, stdin);

    phoneBuffer[strcspn(phoneBuffer, "\n")] = 0;
    strcat(phoneNumber, phoneBuffer);

    printf("\nInsira o seu numero de telefone: ");
    fgets(phoneBuffer, 20, stdin);

    phoneBuffer[strcspn(phoneBuffer, "\n")] = 0;
    strcat(phoneNumber, phoneBuffer);

    printf("\n\n%s", userPhone);
    printf("\n%s", phoneNumber);

    userPhone = phoneNumber;

    return 0;

}

size_t _twilio_null_write(char *ptr, size_t size, size_t nmemb, void *userdata);

int sendMessage(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, bool verbose) {

        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        char url [MAX_MESSAGE_SIZE];

        snprintf(url, sizeof(url), "%s%s%s", "https://api.twilio.com/2010-04-01/Accounts/", account_sid, "/Messages");

        char parameters [MAX_MESSAGE_SIZE];

        snprintf(parameters, sizeof(parameters), "%s%s%s%s%s%s", "To=", to_number, "&From=", from_number, "&Body=", message);



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

size_t _twilio_null_write(char *ptr, size_t size, size_t nmemb, void *userdata) {

    return size * nmemb;

}

int saveUserAccess(const char *username) {

    char data[30];

    FILE *file;

    file = fopen("/etc/pam.d/userAccesses", "a"); 

    snprintf(data, sizeof(data), "\n%s %s", username, userPhone);
    fwrite(data, 1, strlen(data), file);

    fclose(file);

    return 0;

}

int setUseTime(const char *username) {

    char parametersTime[100], time[50];

    strcpy(time, "");

    FILE *file;

    file = fopen("/etc/security/time.conf", "a");

    if(score == 5) {

        strcpy(time, "WeThFrSaSu0000-2400");

    } else if(score == 4) {

        strcpy(time, "WeThFrSaSu0600-2300");

    } else if(score == 3) {

        strcpy(time, "FrSaSu0600-2300");

    } else if(score == 2) {

        strcpy(time, "FrSaSu1000-2100");

    } else if(score == 1) {

        strcpy(time, "SaSu1200-2200");

    } else if(score == 0) {

        strcpy(time, "Su1200-2000");

    }

    snprintf(parametersTime, sizeof(parametersTime), "\nlogin;*;%s;%s", username, time);
    fwrite(parametersTime, 1, strlen(parametersTime), file);

    fclose(file);

    return 0;

}