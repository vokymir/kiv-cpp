//// NOTE: hlavicky relevantni pro jazyk C, ale uz je nechceme v ciste C++ projektu
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//// NOTE: typove nejista konstanta, bylo by dobre pouzit napr. constexpr
#define RECORD_COUNT 100
#define RECORD_COUNT_NEW 1000

struct TRecord
{
	int index;
	double value;
};

/*
 * Vyplni zaznamy v poli magickymi hodnotami
 *
 * recs - ukazatel na pole, ktere ma byt vyplneno
 * count - pocet zaznamu, ktere maji byt vyplnene
 */
void fill_records(struct TRecord* recs, int count)
{
	int i;

	if (!recs)
		return;

	//// NOTE: velice snadne zasahnout za hranice pole, kdyz nekdo nekde uvede "count" vetsi nez je skutencne alokovana
	////       pamet; bylo by lepsi mit compile-time dostupne velikosti, nebo alespon run-time zpusob, jak ziskat
	for (i = 0; i < count; i++)
	{
		recs[i].index = i;
		//// NOTE: surove pretypovani
		recs[i].value = ((double)((i * 19) % 29)) * 0.1854; // nejaka magie, to je fuk
	}
}

/*
 * Vypocte prumer ze zadaneho poctu prvku v poli
 *
 * recs - ukazatel na pole se zaznamy
 * count - pocet zaznamu, ze kterych ma byt udelany prumer
 */
double calculate_average(struct TRecord* recs, int count)
{
	int i;
	double accumulator = 0;

	if (!recs)
		return 0;

	//// NOTE: totez co predtim - chybne zadani "count" = segfault nebo nesmysly
	for (i = 0; i < count; i++)
		accumulator += recs[i].value;

	//// NOTE: pretypovani
	accumulator /= (double)count;

	return accumulator;
}

/* Display all records in an array */
void print_records(const struct TRecord* recs, int count)
{
    if (!recs || count <= 0)
        return;

    for (int i = 0; i < count; i++)
    {
        printf("Record %d: index = %d, value = %.4f\n",
               i, recs[i].index, recs[i].value);
    }
}

int main(int argc, char** argv)
{
	struct TRecord* recs, *recs_tmp;
	double avg;

	//// NOTE: pointer, malloc, alokace surove velikosti pameti, surove pretypovani
	recs = (struct TRecord*)malloc(sizeof(struct TRecord) * RECORD_COUNT);
	if (!recs)
	{
		printf("Nelze alokovat pamet!");
		return 1;
	}

	fill_records(recs, RECORD_COUNT);

	avg = calculate_average(recs, RECORD_COUNT);

	//// NOTE: formatter, ktery na Win a na Linuxu muze fungovat jinak; nebezpecne printf
	printf("Prumer celeho pole je: %f\r\n", avg);

	//// NOTE: nehlidane hranice pole

	avg = calculate_average(recs, RECORD_COUNT / 2);
	printf("Prumer prvni pulky pole je: %f\r\n", avg);
	avg = calculate_average(recs + RECORD_COUNT / 2, RECORD_COUNT / 2);
	printf("Prumer druhe pulky pole je: %f\r\n", avg);



	//// NOTE: realloc, podobne problemy jako vyse s malloc
	////       + presuny pointeru, kdyz se zrovna neco nepovede, nesikovna sprava pameti celkove

	recs_tmp = (struct TRecord*)realloc(recs, sizeof(struct TRecord) * RECORD_COUNT_NEW);
	if (!recs_tmp)
	{
		//// explicitni free() - lepsi by bylo nechat to zaridit RAII (statickou/obalenou alokaci)
		free(recs);
		printf("Nelze alokovat pamet!");
		return 1;
	}
	recs = recs_tmp;

	fill_records(&recs[RECORD_COUNT], RECORD_COUNT_NEW - RECORD_COUNT);

    // print_records(recs, RECORD_COUNT_NEW);

	avg = calculate_average(recs, RECORD_COUNT_NEW);

	//// NOTE: formatter, ktery na Win a na Linuxu muze fungovat jinak; nebezpecne printf
	printf("Prumer celeho pole je: %f\r\n", avg);

	//// NOTE: nehlidane hranice pole

	avg = calculate_average(recs, RECORD_COUNT_NEW / 2);
	printf("Prumer prvni pulky pole je: %f\r\n", avg);
	avg = calculate_average(recs + RECORD_COUNT_NEW / 2, RECORD_COUNT_NEW / 2);
	printf("Prumer druhe pulky pole je: %f\r\n", avg);

	//// NOTE: opet explicitni free() - v malem programu jako je tento to problem neni, ve vetsim meritku by to mohlo zacit cinit potize
	free(recs);

	return 0;
}

