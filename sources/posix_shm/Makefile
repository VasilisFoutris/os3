all: pshm_ucase_bounce pshm_ucase_send

pshm_ucase_bounce: pshm_ucase_bounce.c
	gcc -g -Wall pshm_ucase_bounce.c -o pshm_ucase_bounce -pthread -lrt

pshm_ucase_send: pshm_ucase_send.c
	gcc -g -Wall pshm_ucase_send.c -o pshm_ucase_send -pthread -lrt

semtest_a: semtest_a.c
	gcc -g -Wall semtest_a.c -o semtest_a

clean:
	rm -f pshm_ucase_bounce pshm_ucase_send

