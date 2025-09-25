#include <vector>
#include <iostream>
#include <iomanip>

constexpr int RECORD_COUNT = 100;
constexpr int RECORD_COUNT_NEW = 1000;

struct TRecord
{
    int index;
    double value;
};

/*
 * Vyplni zaznamy v poli magickymi hodnotami
 *
 * recs - vektor, ktery ma byt vyplnen
 * count - pocet zaznamu, ktere maji byt vyplnene
 */
void fill_records(std::vector<TRecord>& recs, int count)
{
    int s = recs.size();

    // puvodne jsem napsal takhle
    // for (int i = recs.size(); i < count; i++)
    // ale array se neshodovala s tou vygenerovanou v puvodnim C programu
    for (int i = 0; i < count - s; i++)
    {
        recs.push_back({
            .index = i,
            .value = ((double)((i * 19) % 29)) * 0.1854
        });
    }
}

/*
 * Vypocte prumer ze zadaneho vectoru
 *
 * recs - reference na vector se zaznamy
 */
double calculate_average(const std::vector<TRecord>& recs)
{
    double avg = 0.0;

    if (recs.empty())
        return avg;

    for (const TRecord& rec : recs)
        avg += rec.value;

    avg /= recs.size();

    return avg;
}

int main(int argc, char **argv)
{
    // koukal jsem, ze jsou i optimalnejsi varianty nez kopirovat si casti vektoru
    // ale jsou v nich pouzity template a iteratory, to jsme jeste nedelali :)
    // a v takhle malem programu...
    std::vector<TRecord> recs, sub_vector;
    double avg;

    fill_records(recs, RECORD_COUNT);

    avg = calculate_average(recs);
    std::cout << std::fixed << std::setprecision(6) << "Prumer celeho pole je: " << avg << std::endl;

    sub_vector = std::vector(recs.begin(), recs.begin() + recs.size() / 2);
    avg = calculate_average(sub_vector);
    std::cout << std::fixed << std::setprecision(6) << "Prumer prvni pulky pole je: " << avg << std::endl;

    sub_vector = std::vector(recs.begin() + recs.size() / 2, recs.end());
    avg = calculate_average(sub_vector);
    std::cout << std::fixed << std::setprecision(6) << "Prumer druhe pulky pole je: " << avg << std::endl;

    fill_records(recs, RECORD_COUNT_NEW);

    avg = calculate_average(recs);
    std::cout << std::fixed << std::setprecision(6) << "Prumer celeho pole je: " << avg << std::endl;

    sub_vector = std::vector(recs.begin(), recs.begin() + recs.size() / 2);
    avg = calculate_average(sub_vector);
    std::cout << std::fixed << std::setprecision(6) << "Prumer prvni pulky pole je: " << avg << std::endl;

    sub_vector = std::vector(recs.begin() + recs.size() / 2, recs.end());
    avg = calculate_average(sub_vector);
    std::cout << std::fixed << std::setprecision(6) << "Prumer druhe pulky pole je: " << avg << std::endl;

    return 0;
}
