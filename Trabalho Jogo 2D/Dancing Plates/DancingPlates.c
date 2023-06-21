#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <time.h>

	#define NUM_PRATOS 8 

	const float FPS = 100;  
	//dimensão da tela
	const int SCREEN_W = 960;
	const int SCREEN_H = 540;

	//largura do jogador
	const float JOGADOR_W = 50;
	const float JOGADOR_H = 100;

	//dimensões dos pratos
	const float PRATO_W = 60;
	const float PRATO_H = 5;

	//dimensões dos postes
	const float POSTE_W = 100;
	const float POSTE_H = 335;
 
	typedef struct Jogador {
		
		float x;
		int equilibrando;
		int mov_esq, mov_dir;
		ALLEGRO_COLOR cor;
		float vel;
		
	} Jogador;

	typedef struct Prato {
		float x,y;
		
		/* um valor entre 0 e 255, em que 255 = prato equilibrado e
		1 = prato com maxima energia, prestes a cair */
		float energia;
		int tempo_a;
		ALLEGRO_COLOR cor;
		int situacao;
	} Prato;

	typedef struct poste{
		int x;
		ALLEGRO_COLOR cor;
	}poste;

	void desenha_cenario() {
		
		ALLEGRO_COLOR BKG_COLOR = al_map_rgb(194,0,223);
		//colore a tela de branco (rgb(255,255,255))
		al_clear_to_color(BKG_COLOR); 
		al_draw_filled_rectangle(0,SCREEN_H-100,SCREEN_W,SCREEN_H,al_map_rgb(255, 170, 0));
	}

	void desenha_final(){

		ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255);
		al_clear_to_color(BKG_COLOR);
	}

	void desenha_jogador(Jogador j) {
		float y_base = SCREEN_H-110;
		al_draw_filled_triangle(j.x, y_base - 90, 
								j.x - JOGADOR_W/2, y_base,
								j.x + JOGADOR_W/2, y_base,
								j.cor);	
		
	}

	void atualizaJogador(Jogador *j) {
		if(j->mov_esq) {
			if(j->x - j->vel > 25)
				j->x -= j->vel;
		}
		if(j->mov_dir) {
			if(j->x + j->vel <= SCREEN_W-25)
				j->x += j->vel;
		}
	}


	void InicializaJogador(Jogador *j) {
		j->x = SCREEN_W / 2;
		j->equilibrando = 0;
		j->cor = al_map_rgb(0, 223, 15);
		j->mov_esq = 0;
		j->mov_dir = 0;
		j->vel = 1.5;
	}


    int geraTempoPrato(int i) {
   	int predefinido[8] = {0};
    int tem = 0;
    int qual, k, j;
    int in[] = {3, 5};

    predefinido[4] = rand() % 5+1;
    tem = predefinido[4];

    qual = in[rand() % 2];
    tem += rand() % 9 + 1;
    predefinido[qual] = tem;

    int in1[3] = {2, 6, 0};
    for (k = 3; k <= 5; k += 2) {
        if (predefinido[k] == 0) {
            in1[k - 3] = k;
        }
    }

    qual = in1[rand() % 3];
    tem += rand() % 9 + 1;
    predefinido[qual] = tem;

    qual = in1[rand() % 3];
    while (predefinido[qual] != 0) {
        qual = in1[rand() % 3];
    }
    tem += rand() % 9 + 1;
    predefinido[qual] = tem;

    for (k = 0; k < 8; k++) {
        if (predefinido[k] == 0) {
            tem += rand() % 9 + 1;
            predefinido[k] = tem;
        }
    }

    return predefinido[i];
}


	void inicializaPratos(Prato pratos[]) {
		int i;
		for(i=0; i < NUM_PRATOS; i++) {
			pratos[i].y = POSTE_W;
			pratos[i].x = 110 +(105*i);
			pratos[i].tempo_a = geraTempoPrato(i);
			pratos[i].energia = 255;
			pratos[i].situacao = 0;
			pratos[i].cor = al_map_rgb(255, 255, 255);
	}
	}
	
	void desenha_pratos(Prato pratos[],int i) {
		al_draw_filled_rectangle(pratos[i].x-(PRATO_W/2),pratos[i].y - PRATO_H, pratos[i].x+ (PRATO_W/2),pratos[i].y + PRATO_H, pratos[i].cor);
	}

	

	void desenha_postes(poste poste[], Jogador j, ALLEGRO_EVENT ev){
		int k;
		for(k = 0; k <= NUM_PRATOS; k++){
		al_draw_line(poste[k].x,POSTE_W,poste[k].x,POSTE_H,poste[k].cor,5);
	}
	}
	void inicializa_postes(poste poste[]){
		int j;
		for (j = 0;j<NUM_PRATOS;j++){
			poste[j].x = 110+(105*j);
			poste[j].cor = al_map_rgb(255, 255, 255);
		}

		
	}
	


int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_AUDIO_STREAM *musica = NULL;
	int recorde;

	FILE * arq;
	arq = fopen("recorde.txt","r");
	fscanf(arq,"%d",&recorde);
	fclose(arq);
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	srand(time(NULL));
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//cria uma tela para exibição dos resultados

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);
	ALLEGRO_FONT *size_60 = al_load_font("arial.ttf", 60, 1);
	if(size_32 == NULL || size_60 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

	//inicializa o modulo que permite ler arquivos wav, ogg etc.
		if(!al_install_audio()){
			fprintf(stderr, "failed to install audio module!\n");
			return -1;
		}

		//inicializa o modulo codec audio
		if(!al_init_acodec_addon()){
			fprintf(stderr, "failed to inialize acodec");
			return -1;
		}

		//defino minha faixa de áudio
		musica = al_load_audio_stream("DancingPlate.wav",4,1024);


 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		al_uninstall_audio();
		al_uninstall_keyboard();
		return -1;
	}

		//registra na fila os eventos de tela (ex: clicar no X na janela)
		al_register_event_source(event_queue, al_get_display_event_source(display));
		//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
		al_register_event_source(event_queue, al_get_timer_event_source(timer));
		//registra na fila os eventos de teclado (ex: pressionar uma tecla)
		al_register_event_source(event_queue, al_get_keyboard_event_source()); 	 
		//registro a faixa de música e a saída de audio padrão do computador
		//al_attach_audio_stream_to_mixer(musica,al_get_default_mixer());
		//defino a música a ser reproduzida e a forma, no caso em looping
		//al_set_audio_stream_playmode(musica,ALLEGRO_PLAYMODE_LOOP);
		//aqui me refiro a tornar a condição verdadeira e iniciar a música
		//al_set_audio_stream_playing(musica, true);
		//muda titulo da tela
		al_set_window_title(display, "Dancing Plates");
		

	//JOGADOR
		Jogador jogador;
		InicializaJogador(&jogador);
		//POSTE
		poste poste[NUM_PRATOS];
		inicializa_postes(poste);

		//PRATOS
		Prato pratos[NUM_PRATOS];
		inicializaPratos(pratos);
		
		//pontuação
		int score = 0;
		int opo = 1;
		int tempo;
	int playing=1;

	//inicia o temporizador
	al_start_timer(timer);

	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			desenha_cenario();

			atualizaJogador(&jogador);

			desenha_jogador(jogador);

			desenha_postes(poste, jogador,ev);

			for(opo = 0; opo<NUM_PRATOS; opo++){
				if(abs(jogador.x - poste[opo].x)<=2 && jogador.equilibrando ==1 && pratos[opo].situacao==1){
					poste[opo].cor = al_map_rgb(rand()%255, rand()%255, rand()%255);
					if(pratos[opo].energia<=255)
					pratos[opo].energia +=1;
					//printf("\nFoi");
					break;
				}
				poste[opo].cor = al_map_rgb(255, 255,255);
			}

			tempo = al_get_timer_count(timer)/FPS;
			for(opo = 0; opo<NUM_PRATOS; opo++){
				if(tempo>=pratos[opo].tempo_a && tempo<=pratos[opo].situacao==0){
					desenha_pratos(pratos,opo);
					pratos[opo].situacao = 1;
				}
				
				}

			for(opo = 0; opo<NUM_PRATOS;opo++){
				if(pratos[opo].situacao==1){
					pratos[opo].cor = al_map_rgb(255,pratos[opo].energia,pratos[opo].energia);
					//printf("\nenergia do prato %d: %2.f", opo, pratos[opo].energia);
					pratos[opo].energia -=0.11;
					if(pratos[opo].energia<=5){
					pratos[opo].y = 440;
					printf("\n%lf",pratos[opo].y);
					}		
					if(pratos[opo].energia <=0){
						playing = 0;
					}	
				}
			}
			score++;
al_draw_textf(size_60, al_map_rgb(0,0,0),SCREEN_W/2 - 30,460, ALLEGRO_ALIGN_CENTER,"Pontuação: %i",score);			

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();

			if(al_get_timer_count(timer)%(int)FPS == 0){
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
				
				
			}

			

			
		}
			
		//se o tipo de evento for o fechamento da tela (clique no x da janela ou apertar esc)
		else if((ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)|| (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
			playing = 0;
		}
		
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 1;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 1;
			} 
			else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
			jogador.equilibrando = 1;

			}

			
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 0;
				
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				jogador.equilibrando = 0;
			}
		}

		if (!playing) {
        desenha_final();

		al_rest(2.0);
			al_set_window_title(display, "Resultados");
			desenha_final();
			if (recorde<score){
				recorde = score;
				al_draw_textf(size_32, al_map_rgb(0,255,0),SCREEN_W/2,SCREEN_H/2, ALLEGRO_ALIGN_CENTER,"Novo Recorde! %i",recorde);
				arq = fopen("recorde.txt","w");
				fprintf(arq,"%d",score);
				fclose(arq);
			}else{
				al_draw_textf(size_32, al_map_rgb(255,0,0),SCREEN_W/2,SCREEN_H/2, ALLEGRO_ALIGN_CENTER,"Sua Pontuação: %i",score);
				al_draw_textf(size_32, al_map_rgb(0,255,0),SCREEN_W/2,SCREEN_H/2+40, ALLEGRO_ALIGN_CENTER,"Recorde: %i",recorde);
			}
			
			
	

        al_flip_display();

        al_rest(5.0);
    }
		}


	

		

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);


	return 0;
}
