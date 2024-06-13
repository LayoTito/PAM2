#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>

#define maxMessageSize 10000
#define MAX_QUESTIONS 5

typedef enum { false, true }    bool;

typedef struct {
	char question[1000];
	char options[4][64];
	int correct_option;
} Question;

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	
    return PAM_SUCCESS;

}

int sendMessage(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, bool verbose);
int startGame(void);

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {

    char phoneNumber[20], phoneBuffer[20], textMessage[20];
    int authCode, userCode;

    strcpy(phoneNumber, "");

	int rval;
	const char *username;

	struct pam_response *resp;
	struct pam_conv *conv;

	rval = pam_get_user(pamh, &username, NULL);

	if (rval != PAM_SUCCESS) {

		return rval;

	}

	if (strcmp("root",username) == 0) {

        startGame();

        printf("\nAntes de realizar o login, é preciso fazer uma verificação");

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


        srand(time(NULL));
        authCode = rand()%(100000 - 999999) + 100000;

        snprintf(textMessage, 100, "O codigo eh: %i", authCode);

        sendMessage("ACdd405d71e1288878b447d34931edde44", "e58595ef4015069f21fe69f054b64a65", textMessage, "+19526495464", phoneNumber, false);

        printf("\n\n\nDigite o codigo recebido: ");
        scanf("%i", &userCode);

        if(userCode == authCode) {

            return PAM_SUCCESS;

        } else {

            return PAM_AUTH_ERR;

        }
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

        snprintf(url, sizeof(url), "%s%s%s", "https://api.twilio.com/2010-04-01/Accounts/", account_sid, "/Messages");

        char parameters[maxMessageSize];

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

void displayQuestion(Question q) {
	printf("%s\n", q.question);
	for (int i = 0; i < 4; i++) {
		printf("%d. %s\n", i + 1, q.options[i]);
	}
}

// function to check the answer
int checkAnswer(Question q, int user_answer) {
	return (user_answer == q.correct_option);
}

// driver code
int startGame() {

	// random number generator
	srand(time(NULL));

	// Initializing questions, options and the correct
	// answer
	Question original_questions[MAX_QUESTIONS] = {
		{ "Ronald Green acaba de ser sequestrado. Sherlock Holmes foi intimado para resolver o caso. Ele encontrou, na cena do crime, um bilhete escrito pela vitima. O papel dizia: �Dois de marco, tres de janeiro, primeiro de agosto�. Sherlock sabia que o nome do sequestrador estava oculto no bilhete. Os suspeitos eram: Ana Green, filha de Ronald. John Jacobson, um funcionario, June Green, esposa e Caitlyn Chara, uma funcionaria. Quem eh o sequestrador?",
		{ "A sequestradora eh June Green", "A sequestradora eh Ana Green", "O sequestrador eh John Jacobson", "A sequestradora eh Caitlyn Chara" },
		2 },
		{ "Uma noite, Sherlock Holmes estava em casa. De repente, uma bola de neve bateu contra sua janela, quebrando o vidro. Ele se levantou e viu quatro meninos da vizinhanca, todos irmaos, correndo numa esquina. Os meninos se chamavam John Crimson, Mark Crimson, Paul Crimson e Rakan Crimson. Apos, Holmes recebeu um bilhete que dizia: �? Crimson. Ele quebrou sua janela�. Qual dos quatro irmaos quebrou a janela?",
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

	// Array of struct data-type
	Question questions[MAX_QUESTIONS];
	memcpy(questions, original_questions,
		sizeof(original_questions));

	int num_questions = MAX_QUESTIONS;

	int score = 0;

	printf("Bem-vindo ao Quiz!\n");

	for (int i = 0; i < MAX_QUESTIONS; i++) {
		int random_index = rand() % num_questions;
		Question current_question = questions[random_index];
		displayQuestion(current_question);

		int user_answer;
		printf("Coloque sua resposta entre (1-4): ");
		scanf("%d", &user_answer);

		if (user_answer >= 1 && user_answer <= 4) {
			if (checkAnswer(current_question,
							user_answer)) {
				printf("Correto!\n");
				score++;
			}
			else {
				printf("Incorreto. A resposta correta eh "
					"%d. %s\n",
					current_question.correct_option,
					current_question.options
						[current_question.correct_option
							- 1]);
			}
		}
		else {
			printf("Escolha invalida. Escolha um numero "
				"entre 1 e 4.\n");
		}

		questions[random_index]
			= questions[num_questions - 1];
		num_questions--;
	}

	printf("Parabens!!!! Quiz completo! Sua "
		"pontuacao foi de: %d/%d\n",
		score, MAX_QUESTIONS);

	return 0;
}