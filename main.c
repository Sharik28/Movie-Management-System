#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 20
#define MAX_USERS 100
#define MAX_PREF 5
#define MAX_VIEW 50
#define MAX_FOLLOW 10

// Structure Declarations
struct tv_show
{
    char name[100];
    char genre[3][15];
    int num_seasons;
    int epi[20];
    int f_day, f_month, f_year;
    int o_day, o_month, o_year;
    char telecast_days[7][10];
    char cast[5][100];
    char producer[2][100];
    char writer[100];
    char prod_company[100];
    char awards[20][100];
    char award_year[20][5];
    char receipant[20][100];
    int count_awards;
};

struct Cast
{
    char name[50];
    char shows[SIZE][50];
    int count_shows;
    char movies[SIZE][50];
    int count_movies;
    char awards[SIZE][50];
    char awards_year[SIZE][5];
    int count_awards;
};

struct movies
{
    char name[100];
    int r_day, r_month, r_year;
    char cast[5][100];
    char producer[100];
    char writer[100];
    char soundtrack[4][50];
    char prod_company[100];
    int budget;
    int collection;
    char awards[20][100];
    char year[20][5];
    char receipant[20][100];
    int count_awards;
};

struct Subscription {
    int start_day, start_month, start_year;
    int end_day, end_month, end_year;
    char plan[20];
    float amount;
};

struct Viewing {
    char title[100];
    char type[10];
    float hours_watched;
    int rating;
};

struct User {
    int id;
    char name[50];
    int reg_day, reg_month, reg_year;
    struct Subscription subs;
    char preferences[5][50];
    int pref_count;
    struct Viewing views[50];
    int view_count;
    char subscribed_shows[10][100];
    int show_count;
    char following[10][50];
    int follow_count;
};

// Function Declarartions 
void updateCastDatabaseFromMovie(struct movies info);
void updateCastDatabaseFromShow(struct tv_show info);
void input_m_file(struct movies temp);
int check_tvawards(char award_name[100], char receipant[100], char award_year[5], int f);
int check_tvshow(char search_name[100], int filetype);
void print_movies(char search_name[100]);
void get_today_date(int *day, int *month, int *year);
int find_matching_users(char prefs[5][50], int pref_count);
void user_to_file(struct User u);
void find_content_by_preferences(char *filename, char *type, struct User *u);
void add_user();
void subscribed_cont();
void categorize_user();
int similarName(const char *a, const char *b);
void to_lower(char *s);
void main_menu();
void add_update_menu();
void search_menu();
void sort_menu();
void other_menu();

// Helping functions
void get_today_date(int *day, int *month, int *year) 
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    *day = tm_info->tm_mday;
    *month = tm_info->tm_mon + 1;
    *year = tm_info->tm_year + 1900;
}

void to_lower(char *s) 
{
    for (int i = 0; s[i]; i++) s[i] = tolower(s[i]);
}

int similarName(const char *a, const char *b)
{
    int lenA = strlen(a), lenB = strlen(b);
    if (abs(lenA - lenB) > 1)
        return 0; // names differ too much in length
    int diff = 0;
    for (int i = 0; a[i] && b[i]; i++)
    {
        if (tolower(a[i]) != tolower(b[i]))
            diff++;
        if (diff > 1)
            return 0; // more than one char different
    }
    return 1; // names are similar enough
}

void normalizeName(char *name)
{
    int capitalize = 1;
    for (int i = 0; name[i]; i++)
    {
        if (isspace(name[i]))
        {
            capitalize = 1;
        }
        else if (capitalize)
        {
            name[i] = toupper(name[i]);
            capitalize = 0;
        }
        else
        {
            name[i] = tolower(name[i]);
        }
    }
}

// tv show section 
void print_tvinfo(char search_name[100])
{
    FILE *tvp;
    char line[256];
    int found = 0;

    tvp = fopen("tv_shows.txt", "r");
    if (tvp == NULL)
    {
        printf("ERROR : unable to open File !");
        return;
    }
    printf("\n---------- TV SHOW DETAILS ----------\n");
    while (fgets(line, sizeof(line), tvp))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            char temp_name[100];
            sscanf(line, "Name: %[^\n]", temp_name);
            if (_stricmp(temp_name, search_name) == 0)
            {
                found = 1;
                printf("%s", line);
            }
            else
                found = 0;
        }
        else if (found)
        {
            if (strstr(line, "-------------------------------------"))
            {
                printf("-------------------------------------\n");
                break;
            }
            printf("%s", line);
        }
    }
    if (!found)
    {
        printf("No details found for %s.\n", search_name);
    }
    fclose(tvp);
}

int check_tvshow(char search_name[100], int filetype)
{
    FILE *tfp;
    char filename[100];
    if(filetype == 1)
        strcpy(filename, "tv_shows.txt");
    else if(filetype == 2) 
        strcpy(filename, "Movies.txt");
    tfp = fopen(filename , "r");
    if (tfp == NULL)
    {
        printf("Error: Could not open tv_shows.txt\n");
        return 0; // assume not found
    }

    char line[256];
    char temp_name[100];

    while (fgets(line, sizeof(line), tfp))
    {
        char *p = line;
        while (isspace((unsigned char)*p))
            p++;

        if (strncasecmp(p, "Name:", 5) == 0)
        {
            sscanf(p, "Name: %[^\n]", temp_name); // extract until newline character
            temp_name[strcspn(temp_name, "\n")] = '\0';

            if (strcasecmp(temp_name, search_name) == 0)
            {
                fclose(tfp);
                return 1; // found
            }
        }
    }

    fclose(tfp);
    return 0; // not found
}

void input_tvshows()
{
    struct tv_show info = {0};
    int i, j;

    printf("---------- TV SHOW DETAILS ----------\n");
    printf("Name: ");
    fgets(info.name, sizeof(info.name), stdin);
    info.name[strcspn(info.name, "\n")] = '\0';

    if (check_tvshow(info.name, 1) == 1)
    {
        printf("TV show already exists.\n");
        printf("Want to view details? (yes/no): ");
        char temp[5];
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strncasecmp(temp, "yes", 3) == 0)
            print_tvinfo(info.name);
        return;
    }

    printf("Enter 3 genres:\n");
    for (i = 0; i < 3; i++)
    {
        printf("Genre %d: ", i + 1);
        fgets(info.genre[i], sizeof(info.genre[i]), stdin);
        info.genre[i][strcspn(info.genre[i], "\n")] = '\0';
    }

    printf("No. of Seasons: ");
    scanf("%d", &info.num_seasons);
    getchar();

    printf("Enter number of episodes for each season:\n");
    for (i = 0; i < info.num_seasons; i++)
    {
        printf("Season %d: ", i + 1);
        scanf("%d", &info.epi[i]);
    }
    getchar();

    printf("Enter first telecast date (dd mm yyyy): ");
    scanf("%d %d %d", &info.f_day, &info.f_month, &info.f_year);
    getchar();

    printf("Enter original release date (dd mm yyyy): ");
    scanf("%d %d %d", &info.o_day, &info.o_month, &info.o_year);
    getchar();

    printf("Enter telecast days (type 'done' to stop):\n");
    for (i = 0; i < 7; i++)
    {
        printf("Day %d: ", i + 1);
        fgets(info.telecast_days[i], sizeof(info.telecast_days[i]), stdin);
        info.telecast_days[i][strcspn(info.telecast_days[i], "\n")] = '\0';
        if (strcasecmp(info.telecast_days[i], "done") == 0)
            break;
    }

    printf("Enter cast (type 'done' to stop):\n");
    for (i = 0; i < 5; i++)
    {
        printf("Cast %d: ", i + 1);
        fgets(info.cast[i], sizeof(info.cast[i]), stdin);
        info.cast[i][strcspn(info.cast[i], "\n")] = '\0';
        if (strcasecmp(info.cast[i], "done") == 0)
            break;
    }

    printf("Enter producer (type 'done' to stop):\n");
    for (i = 0; i < 2; i++)
    {
        printf("Producer %d: ", i + 1);
        fgets(info.producer[i], sizeof(info.producer[i]), stdin);
        info.producer[i][strcspn(info.producer[i], "\n")] = '\0';
        if (strcasecmp(info.producer[i], "done") == 0)
            break;
    }

    printf("Enter writer name: ");
    fgets(info.writer, sizeof(info.writer), stdin);
    info.writer[strcspn(info.writer, "\n")] = '\0';

    printf("Enter production company: ");
    fgets(info.prod_company, sizeof(info.prod_company), stdin);
    info.prod_company[strcspn(info.prod_company, "\n")] = '\0';

    printf("Enter Award Details (type 'done' to stop):\n");
    info.count_awards = 0;

    for (i = 0; i < 20; i++)
    {
        printf("Award %d: ", i + 1);
        fgets(info.awards[i], sizeof(info.awards[i]), stdin);
        info.awards[i][strcspn(info.awards[i], "\n")] = '\0';
        if (strcasecmp(info.awards[i], "done") == 0)
            break;

        printf("Year: ");
        fgets(info.award_year[i], sizeof(info.award_year[i]), stdin);
        info.award_year[i][strcspn(info.award_year[i], "\n")] = '\0';
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        printf("Recipient: ");
        fgets(info.receipant[i], sizeof(info.receipant[i]), stdin);
        info.receipant[i][strcspn(info.receipant[i], "\n")] = '\0';

        if (strlen(info.awards[i]) == 0 || strlen(info.receipant[i]) == 0)
        {
            printf(" Empty input detected. Please re-enter this award.\n");
            i--; // retry same index
            continue;
        }

        // Check for duplicates
        int awardCheck = check_tvawards(info.awards[i], info.receipant[i], info.award_year[i], 1);

        if (awardCheck == 1)
        {
            printf("Same recipient already received '%s' (%s). Adding show under same award.\n",
                   info.awards[i], info.award_year[i]);
        }
        else if (awardCheck == 2)
        {
            printf("ERROR: '%s' (%s) already assigned to another recipient.\n",
                   info.awards[i], info.award_year[i]);
            printf("Please re-enter this award.\n");
            i--; // let user re-enter
            continue;
        }
        info.count_awards++;
    }

    // --- WRITE TO FILE ---//
    FILE *fp = fopen("tv_shows.txt", "a");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    normalizeName(info.name);
    fprintf(fp, "---------- TV SHOW DETAILS ----------\n");
    fprintf(fp, "Name: %s\n", info.name);

    fprintf(fp, "Genres: ");
    for (i = 0; i < 3; i++)
        fprintf(fp, "%s ", info.genre[i]);
    fprintf(fp, "\n");

    fprintf(fp, "Seasons: %d\n", info.num_seasons);
    for (i = 0; i < info.num_seasons; i++)
        fprintf(fp, "  Season %d: %d episodes\n", i + 1, info.epi[i]);

    fprintf(fp, "First Telecast Date: %02d-%02d-%04d\n", info.f_day, info.f_month, info.f_year);
    fprintf(fp, "Original Release Date: %02d-%02d-%04d\n", info.o_day, info.o_month, info.o_year);

    fprintf(fp, "Telecast Days: ");
    for (i = 0; i < 7 && strcasecmp(info.telecast_days[i], "done") != 0; i++)
        fprintf(fp, "%s ", info.telecast_days[i]);
    fprintf(fp, "\n");

    fprintf(fp, "Cast: ");
    for (i = 0; i < 5 && strcasecmp(info.cast[i], "done") != 0; i++)
        fprintf(fp, "%s ", info.cast[i]);
    fprintf(fp, "\n");

    fprintf(fp, "Producers: ");
    for (i = 0; i < 2 && strcasecmp(info.producer[i], "done") != 0; i++)
        fprintf(fp, "%s ", info.producer[i]);
    fprintf(fp, "\n");

    fprintf(fp, "Writer: %s\n", info.writer);
    fprintf(fp, "Production Company: %s\n", info.prod_company);

    if (info.count_awards > 0)
    {
        fprintf(fp, "Awards:\n");
        for (i = 0; i < info.count_awards; i++)
        {
            fprintf(fp, "  %s (%s) — %s\n", info.awards[i], info.award_year[i], info.receipant[i]);
        }
    }

    fprintf(fp, "-------------------------------------\n\n");
    fclose(fp);

    updateCastDatabaseFromShow(info);

    printf("TV Show '%s' added successfully!\n", info.name);
}

int check_tvawards(char award_name[100], char receipant[100], char award_year[5], int f)
{
    FILE *chkfp;
    if (f == 1)
        chkfp = fopen("tv_shows.txt", "r");
    else
        chkfp = fopen("Movies.txt", "r");

    if (chkfp == NULL)
    {
        printf("Error Opening file!\n");
        return -1;
    }

    char line[256];
    char temp_award[100], temp_year[10], temp_receipant[100];

    while (fgets(line, sizeof(line), chkfp))
    {
        // only process lines that look like awards
        if (strstr(line, "(") && (strstr(line, "—") || strstr(line, "-")))
        {
            // try to extract award, year, and recipient (handle both dash styles)
            int matched = 0;
            if (sscanf(line, " %[^'(] (%[^)]) — %[^\n]", temp_award, temp_year, temp_receipant) == 3)
                matched = 1;
            else if (sscanf(line, " %[^'(] (%[^)]) - %[^\n]", temp_award, temp_year, temp_receipant) == 3)
                matched = 1;

            if (!matched)
                continue; // skip malformed lines

            // remove any extra whitespace/newlines
            temp_award[strcspn(temp_award, "\n")] = '\0';
            temp_year[strcspn(temp_year, "\n")] = '\0';
            temp_receipant[strcspn(temp_receipant, "\n")] = '\0';

            // trim spaces (left/right)
            char *start, *end;
            start = temp_award;
            while (isspace((unsigned char)*start))
                start++;
            end = start + strlen(start) - 1;
            while (end > start && isspace((unsigned char)*end))
                *end-- = '\0';
            strcpy(temp_award, start);

            start = temp_receipant;
            while (isspace((unsigned char)*start))
                start++;
            end = start + strlen(start) - 1;
            while (end > start && isspace((unsigned char)*end))
                *end-- = '\0';
            strcpy(temp_receipant, start);

            // convert all strings to lowercase for comparison
            for (int i = 0; temp_award[i]; i++)
                temp_award[i] = tolower(temp_award[i]);
            for (int i = 0; temp_year[i]; i++)
                temp_year[i] = tolower(temp_year[i]);
            for (int i = 0; temp_receipant[i]; i++)
                temp_receipant[i] = tolower(temp_receipant[i]);

            char a_name[100], a_year[10], a_rec[100];
            strcpy(a_name, award_name);
            strcpy(a_year, award_year);
            strcpy(a_rec, receipant);
            for (int i = 0; a_name[i]; i++)
                a_name[i] = tolower(a_name[i]);
            for (int i = 0; a_year[i]; i++)
                a_year[i] = tolower(a_year[i]);
            for (int i = 0; a_rec[i]; i++)
                a_rec[i] = tolower(a_rec[i]);

            // ✅ same award + year + recipient → valid (same person got same award)
            if (strcmp(a_name, temp_award) == 0 &&
                strcmp(a_year, temp_year) == 0 &&
                strcmp(a_rec, temp_receipant) == 0)
            {
                fclose(chkfp);
                return 1; // same recipient (allowed)
            }

            // ❌ same award + year but different recipient → reject
            if (strcmp(a_name, temp_award) == 0 &&
                strcmp(a_year, temp_year) == 0 &&
                strcmp(a_rec, temp_receipant) != 0)
            {
                fclose(chkfp);
                return 2; // duplicate award assigned to another person
            }
        }
    }

    fclose(chkfp);
    return 0; // no duplicate found (safe to add)
}

// movie section
void input_movies()
{
    struct movies info = {0};
    int i;

    printf("------------ Movie Details ------------\n");

    printf("Movie Name: ");
    fgets(info.name, sizeof(info.name), stdin);
    info.name[strcspn(info.name, "\n")] = '\0';
    normalizeName(info.name);

    if (check_tvshow(info.name, 2) == 1)
    {
        printf("Movie already exists.\n");
        printf("Want to view details? (yes/no): ");
        char temp[5];
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strncasecmp(temp, "yes", 3) == 0)
            print_movies(info.name);
        return;
    }

    printf("Release Date (dd mm yyyy): ");
    scanf("%d %d %d", &info.r_day, &info.r_month, &info.r_year);
    getchar(); // clear leftover newline

    printf("Cast (enter 'done' to stop):\n");
    for (i = 0; i < 5; i++)
    {
        printf("Cast %d: ", i + 1);
        fgets(info.cast[i], sizeof(info.cast[i]), stdin);
        info.cast[i][strcspn(info.cast[i], "\n")] = '\0';
        if (strcasecmp(info.cast[i], "done") == 0)
            break;
    }

    printf("Producer: ");
    fgets(info.producer, sizeof(info.producer), stdin);
    info.producer[strcspn(info.producer, "\n")] = '\0';

    printf("Writer: ");
    fgets(info.writer, sizeof(info.writer), stdin);
    info.writer[strcspn(info.writer, "\n")] = '\0';

    printf("Soundtracks (enter 'done' to stop):\n");
    for (i = 0; i < 4; i++)
    {
        printf("Track %d: ", i + 1);
        fgets(info.soundtrack[i], sizeof(info.soundtrack[i]), stdin);
        info.soundtrack[i][strcspn(info.soundtrack[i], "\n")] = '\0';
        if (strcasecmp(info.soundtrack[i], "done") == 0)
            break;
    }

    printf("Production Company: ");
    fgets(info.prod_company, sizeof(info.prod_company), stdin);
    info.prod_company[strcspn(info.prod_company, "\n")] = '\0';

    printf("Budget (in Crores): ");
    scanf("%d", &info.budget);
    printf("World-wide Collection (in Crores): ");
    scanf("%d", &info.collection);
    getchar(); // clear newline

    printf("Awards Received (enter 'done' to stop)\n");
    info.count_awards = 0;

    for (i = 0; i < 20; i++)
    {
        printf("Award %d Name: ", i + 1);
        fgets(info.awards[i], sizeof(info.awards[i]), stdin);
        info.awards[i][strcspn(info.awards[i], "\n")] = '\0';
        if (strcasecmp(info.awards[i], "done") == 0)
            break;

        printf("Year: ");
        fgets(info.year[i], sizeof(info.year[i]), stdin);
        info.year[i][strcspn(info.year[i], "\n")] = '\0';

        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        printf("Recipient: ");
        fgets(info.receipant[i], sizeof(info.receipant[i]), stdin);
        info.receipant[i][strcspn(info.receipant[i], "\n")] = '\0';

        if (strlen(info.awards[i]) == 0 || strlen(info.receipant[i]) == 0)
        {
            printf("Empty input detected. Please re-enter this award.\n");
            i--; // retry
            continue;
        }

        int awardCheck = check_tvawards(info.awards[i], info.receipant[i], info.year[i], 2);

        if (awardCheck == 1)
        {
            printf("Same recipient already received '%s' (%s). Allowed for new movie.\n",
                   info.awards[i], info.year[i]);
        }
        else if (awardCheck == 2)
        {
            printf("ERROR: '%s' (%s) already assigned to another recipient.\n",
                   info.awards[i], info.year[i]);
            printf("Please re-enter this award.\n");
            i--;
            continue;
        }

        info.count_awards++;
    }
    for (int i = 0; i < 5; i++)
        normalizeName(info.cast[i]);

    input_m_file(info);
    updateCastDatabaseFromMovie(info);
}

void input_m_file(struct movies temp)
{
    int i;
    FILE *mfp = fopen("Movies.txt", "a");
    if (mfp == NULL)
    {
        printf("ERROR Opening Movies.txt file!\n");
        return;
    }
    normalizeName(temp.name);
    fprintf(mfp, "---------- MOVIE DETAILS ----------\n");
    fprintf(mfp, "Name: %s\n", temp.name);
    fprintf(mfp, "Release Date: %02d-%02d-%04d\n", temp.r_day, temp.r_month, temp.r_year);

    fprintf(mfp, "Cast: ");
    for (i = 0; i < 5; i++)
    {
        if (strcasecmp(temp.cast[i], "done") == 0)
            break;
        fprintf(mfp, "%s, ", temp.cast[i]);
    }
    fprintf(mfp, "\n");

    fprintf(mfp, "Producer: %s\n", temp.producer);
    fprintf(mfp, "Writer: %s\n", temp.writer);

    fprintf(mfp, "Soundtracks:\n");
    for (i = 0; i < 4; i++)
    {
        if (strcasecmp(temp.soundtrack[i], "done") == 0)
            break;
        fprintf(mfp, "  %s\n", temp.soundtrack[i]);
    }

    fprintf(mfp, "Production Company: %s\n", temp.prod_company);
    fprintf(mfp, "Budget (in crores): %d\n", temp.budget);
    fprintf(mfp, "World-wide Collection (in crores): %d\n", temp.collection);

    fprintf(mfp, "Awards:\n");
    for (i = 0; i < 4; i++)
    {
        if (strcasecmp(temp.awards[i], "done") == 0)
            break;
        fprintf(mfp, "  %s\n", temp.awards[i]);
    }

    fprintf(mfp, "-------------------------------------\n");
    fclose(mfp);

    FILE *cfp = fopen("cast_info.txt", "a");
    if (cfp == NULL)
    {
        printf("ERROR Opening Cast.txt file!\n");
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (strcasecmp(temp.cast[i], "done") == 0)
            break;
        fprintf(cfp, "Cast Name: %s\n", temp.cast[i]);
        fprintf(cfp, "Movie: %s\n", temp.name);
        fprintf(cfp, "-------------------------------------\n");
    }

    fclose(cfp);

    printf("\nMovie '%s' successfully stored in Movies.txt\n", temp.name);
}

void print_movies(char search_name[100])
{
    FILE *mfp;
    char line[256];
    int found = 0;

    mfp = fopen("Movies.txt", "r");
    if (mfp == NULL)
    {
        printf("Error: Could not open Movies.txt\n");
        return;
    }

    while (fgets(line, sizeof(line), mfp))
    {
        if (strncasecmp(line, "Name:", 5) == 0)
        {
            char movie_name[100];
            strcpy(movie_name, line + 6);
            movie_name[strcspn(movie_name, "\n")] = '\0';

            if (strcasecmp(movie_name, search_name) == 0)
            {
                found = 1;
                printf("---------- Movie Details ----------\n");
                printf("Name: %s\n", movie_name);

                while (fgets(line, sizeof(line), mfp))
                {
                    printf("%s", line);
                    if (strstr(line, "-------------------------------------") != NULL)
                        break;
                }
                break;
            }
        }
    }

    fclose(mfp);

    if (!found)
        printf("Movie '%s' not found in file.\n", search_name);
}

// cast section
void addCast()
{
    struct Cast newCast = {0};
    printf("Enter the name of cast: ");
    fgets(newCast.name, sizeof(newCast.name), stdin);
    newCast.name[strcspn(newCast.name, "\n")] = '\0';
    normalizeName(newCast.name);

    printf("\nTV Shows (enter 'done' to stop):\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("Show %d: ", i + 1);
        fgets(newCast.shows[i], sizeof(newCast.shows[i]), stdin);
        newCast.shows[i][strcspn(newCast.shows[i], "\n")] = '\0';
        if (strcasecmp(newCast.shows[i], "done") == 0)
            break;
        newCast.count_shows++;
    }

    printf("\nMovies (enter 'done' to stop):\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("Movie %d: ", i + 1);
        fgets(newCast.movies[i], sizeof(newCast.movies[i]), stdin);
        newCast.movies[i][strcspn(newCast.movies[i], "\n")] = '\0';
        if (strcasecmp(newCast.movies[i], "done") == 0)
            break;
        newCast.count_movies++;
    }

    printf("\nAwards (enter 'done' to stop):\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("Award %d: ", i + 1);
        if (!fgets(newCast.awards[i], sizeof(newCast.awards[i]), stdin))
        {
            newCast.awards[i][0] = '\0';
        }
        newCast.awards[i][strcspn(newCast.awards[i], "\n")] = '\0';
        if (strcasecmp(newCast.awards[i], "done") == 0)
            break;

        // guard against empty award name
        if (strlen(newCast.awards[i]) == 0)
        {
            printf("Empty award name detected. Please re-enter.\n");
            i--;
            continue;
        }

        printf("Year for Award %d: ", i + 1);
        if (!fgets(newCast.awards_year[i], sizeof(newCast.awards_year[i]), stdin))
        {
            newCast.awards_year[i][0] = '\0';
        }
        newCast.awards_year[i][strcspn(newCast.awards_year[i], "\n")] = '\0';

        // guard against empty year
        if (strlen(newCast.awards_year[i]) == 0)
        {
            printf("Empty year detected. Please re-enter this award.\n");
            // clear any leftover input just in case, then retry this index
            int _ch;
            while ((_ch = getchar()) != '\n' && _ch != EOF)
                ;
            i--;
            continue;
        }

        // Cross-verify award in TV and Movie databases
        int awardCheckTV = check_tvawards(newCast.awards[i], newCast.name, newCast.awards_year[i], 1);
        int awardCheckMovie = check_tvawards(newCast.awards[i], newCast.name, newCast.awards_year[i], 2);

        if (awardCheckTV == 2 || awardCheckMovie == 2)
        {
            printf("ERROR: '%s' (%s) already assigned to another recipient.\n", newCast.awards[i], newCast.awards_year[i]);
            printf("Please re-enter this award.\n");
            // clear stdin (very important so next fgets waits properly)
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            i--; // retry same index
            continue;
        }
        else if (awardCheckTV == 1 || awardCheckMovie == 1)
        {
            printf("Same recipient already received '%s' (%s) — allowed.\n", newCast.awards[i], newCast.awards_year[i]);
        }
        else
        {
            printf("New award '%s' (%s) verified successfully.\n", newCast.awards[i], newCast.awards_year[i]);
        }

        newCast.count_awards++;
    }

    FILE *fp = fopen("cast_info.txt", "a");
    if (!fp)
    {
        printf("Error: Unable to open cast_info.txt\n");
        return;
    }

    fprintf(fp, "------------ Cast Details ------------\n");
    fprintf(fp, "Cast Name: %s\n", newCast.name);

    fprintf(fp, "No. TV Shows : %d\n", newCast.count_shows);
    fprintf(fp, "TV Shows:\n");
    for (int i = 0; i < newCast.count_shows; i++)
        fprintf(fp, "  - %s\n", newCast.shows[i]);

    fprintf(fp, "No. Movies : %d\n", newCast.count_movies);
    fprintf(fp, "Movies:\n");
    for (int i = 0; i < newCast.count_movies; i++)
        fprintf(fp, "  - %s\n", newCast.movies[i]);

    fprintf(fp, "No. Awards : %d\n", newCast.count_awards);
    fprintf(fp, "Awards:\n");
    for (int i = 0; i < newCast.count_awards; i++)
        fprintf(fp, "  - %s (%s)\n", newCast.awards[i], newCast.awards_year[i]);

    fprintf(fp, "--------------------------------------\n\n");
    fclose(fp);

    printf("\nCast '%s' added successfully and stored in file!\n", newCast.name);
}

void print_cast()
{
    char temp_name[50];
    char line[256];
    int found = 0;
    FILE *cfp = fopen("cast_info.txt", "r");

    printf("Enter Cast Name for Details: ");
    fgets(temp_name, sizeof(temp_name), stdin);
    temp_name[strcspn(temp_name, "\n")] = '\0';
    if (cfp == NULL)
    {
        printf("Error Opening file: ");
        return;
    }
    while (fgets(line, sizeof(line), cfp))
    {
        if (strncasecmp(line, "Cast Name:", 10) == 0)
        {
            char cast_name[100];
            strcpy(cast_name, line + 11);
            cast_name[strcspn(cast_name, "\n")] = '\0';

            if (strcasecmp(cast_name, temp_name) == 0)
            {
                found = 1;
                printf("---------- Cast Details ----------\n");
                printf("Cast Name: %s\n", cast_name);

                while (fgets(line, sizeof(line), cfp))
                {
                    printf("%s", line);
                    if (strstr(line, "--------------------------------------") != NULL)
                        break;
                }
                break;
            }
        }
    }
}

void updateCastDatabaseFromShow(struct tv_show info)
{
    FILE *fp = fopen("cast_info.txt", "r");
    struct Cast existing[100] = {0};
    int cast_count = 0;

    // Step 1: Parse existing cast_info.txt cleanly
    if (fp)
    {
        char line[256];
        int in_shows = 0, in_movies = 0, in_awards = 0;

        while (fgets(line, sizeof(line), fp))
        {
            if (strstr(line, "------------ Cast Details ------------"))
            {
                in_shows = in_movies = in_awards = 0;
                continue;
            }
            else if (strncasecmp(line, "Cast Name:", 10) == 0)
            {
                sscanf(line, "Cast Name: %[^\n]", existing[cast_count].name);
            }
            else if (strncasecmp(line, "TV Shows:", 9) == 0)
            {
                in_shows = 1;
                in_movies = in_awards = 0;
            }
            else if (strncasecmp(line, "Movies:", 7) == 0)
            {
                in_movies = 1;
                in_shows = in_awards = 0;
            }
            else if (strncasecmp(line, "Awards:", 7) == 0)
            {
                in_awards = 1;
                in_shows = in_movies = 0;
            }
            else if (strstr(line, "--------------------------------------"))
            {
                cast_count++;
                in_shows = in_movies = in_awards = 0;
            }
            else if (strstr(line, "  - "))
            {
                char item[100];
                sscanf(line, "  - %[^\n]", item);

                if (in_shows)
                    strcpy(existing[cast_count].shows[existing[cast_count].count_shows++], item);
                else if (in_movies)
                    strcpy(existing[cast_count].movies[existing[cast_count].count_movies++], item);
                else if (in_awards)
                {
                    char award[100], year[10];
                    if (sscanf(item, "%[^'('](%[^')'])", award, year) == 2)
                    {
                        strcpy(existing[cast_count].awards[existing[cast_count].count_awards], award);
                        strcpy(existing[cast_count].awards_year[existing[cast_count].count_awards], year);
                        existing[cast_count].count_awards++;
                    }
                    else
                    {
                        strcpy(existing[cast_count].awards[existing[cast_count].count_awards++], item);
                    }
                }
            }
        }
        fclose(fp);
    }

    // Step 2: Normalize new input
    for (int i = 0; i < 5; i++)
        normalizeName(info.cast[i]);
    normalizeName(info.name);

    // Step 3: Merge or add cast entries
    for (int i = 0; i < 5; i++)
    {
        if (strlen(info.cast[i]) == 0 || strcasecmp(info.cast[i], "done") == 0)
            continue;

        int found = -1;
        for (int j = 0; j < cast_count; j++)
        {
            if (strcasecmp(existing[j].name, info.cast[i]) == 0 || similarName(existing[j].name, info.cast[i]))
            {
                found = j;
                break;
            }
        }

        if (found == -1)
        {
            // New cast
            strcpy(existing[cast_count].name, info.cast[i]);
            existing[cast_count].count_shows = 1;
            strcpy(existing[cast_count].shows[0], info.name);
            existing[cast_count].count_movies = 0;
            existing[cast_count].count_awards = 0;

            for (int j = 0; j < info.count_awards; j++)
            {
                if (strcasecmp(info.receipant[j], info.cast[i]) == 0)
                {
                    strcpy(existing[cast_count].awards[existing[cast_count].count_awards], info.awards[j]);
                    strcpy(existing[cast_count].awards_year[existing[cast_count].count_awards], info.award_year[j]);
                    existing[cast_count].count_awards++;
                }
            }
            cast_count++;
        }
        else
        {
            // Existing cast → merge new show + awards
            int dup = 0;
            for (int k = 0; k < existing[found].count_shows; k++)
                if (strcasecmp(existing[found].shows[k], info.name) == 0)
                    dup = 1;

            if (!dup)
                strcpy(existing[found].shows[existing[found].count_shows++], info.name);

            for (int j = 0; j < info.count_awards; j++)
            {
                if (strcasecmp(info.receipant[j], info.cast[i]) == 0)
                {
                    int exists = 0;
                    for (int k = 0; k < existing[found].count_awards; k++)
                    {
                        if (strcasecmp(existing[found].awards[k], info.awards[j]) == 0 &&
                            strcasecmp(existing[found].awards_year[k], info.award_year[j]) == 0)
                        {
                            exists = 1;
                            break;
                        }
                    }
                    if (!exists)
                    {
                        strcpy(existing[found].awards[existing[found].count_awards], info.awards[j]);
                        strcpy(existing[found].awards_year[existing[found].count_awards], info.award_year[j]);
                        existing[found].count_awards++;
                    }
                }
            }
        }
    }

    // Step 4: Write clean updated file
    fp = fopen("cast_info.txt", "w");
    if (!fp)
    {
        printf("Error writing to cast_info.txt\n");
        return;
    }

    for (int i = 0; i < cast_count; i++)
    {
        fprintf(fp, "------------ Cast Details ------------\n");
        fprintf(fp, "Cast Name: %s\n", existing[i].name);
        fprintf(fp, "No. TV Shows : %d\n", existing[i].count_shows);
        fprintf(fp, "TV Shows:\n");
        for (int j = 0; j < existing[i].count_shows; j++)
            fprintf(fp, "  - %s\n", existing[i].shows[j]);

        fprintf(fp, "No. Movies : %d\n", existing[i].count_movies);
        fprintf(fp, "Movies:\n");
        for (int j = 0; j < existing[i].count_movies; j++)
            fprintf(fp, "  - %s\n", existing[i].movies[j]);

        fprintf(fp, "No. Awards : %d\n", existing[i].count_awards);
        fprintf(fp, "Awards:\n");
        for (int j = 0; j < existing[i].count_awards; j++)
            fprintf(fp, "  - %s (%s)\n", existing[i].awards[j], existing[i].awards_year[j]);

        fprintf(fp, "--------------------------------------\n\n");
    }

    fclose(fp);
    printf("Cast database updated successfully (TV Shows merged cleanly).\n");
}

void updateCastDatabaseFromMovie(struct movies info)
{
    FILE *fp = fopen("cast_info.txt", "r");
    struct Cast existing[100] = {0};
    int cast_count = 0;

    // Step 1: Load existing cast from file (properly track context)
    if (fp)
    {
        char line[256];
        int in_shows = 0, in_movies = 0, in_awards = 0;

        while (fgets(line, sizeof(line), fp))
        {
            if (strstr(line, "------------ Cast Details ------------"))
            {
                in_shows = in_movies = in_awards = 0;
                continue;
            }
            else if (strncasecmp(line, "Cast Name:", 10) == 0)
            {
                sscanf(line, "Cast Name: %[^\n]", existing[cast_count].name);
            }
            else if (strncasecmp(line, "TV Shows:", 9) == 0)
            {
                in_shows = 1;
                in_movies = in_awards = 0;
            }
            else if (strncasecmp(line, "Movies:", 7) == 0)
            {
                in_movies = 1;
                in_shows = in_awards = 0;
            }
            else if (strncasecmp(line, "Awards:", 7) == 0)
            {
                in_awards = 1;
                in_shows = in_movies = 0;
            }
            else if (strstr(line, "--------------------------------------"))
            {
                cast_count++;
                in_shows = in_movies = in_awards = 0;
            }
            else if (strstr(line, "  - "))
            {
                char item[100];
                sscanf(line, "  - %[^\n]", item);

                if (in_shows)
                    strcpy(existing[cast_count].shows[existing[cast_count].count_shows++], item);
                else if (in_movies)
                    strcpy(existing[cast_count].movies[existing[cast_count].count_movies++], item);
                else if (in_awards)
                {
                    char award[100], year[10];
                    if (sscanf(item, "%[^'('](%[^')'])", award, year) == 2)
                    {
                        strcpy(existing[cast_count].awards[existing[cast_count].count_awards], award);
                        strcpy(existing[cast_count].awards_year[existing[cast_count].count_awards], year);
                        existing[cast_count].count_awards++;
                    }
                    else
                    {
                        strcpy(existing[cast_count].awards[existing[cast_count].count_awards++], item);
                    }
                }
            }
        }
        fclose(fp);
    }

    // Step 2: Normalize all cast names
    for (int i = 0; i < 5; i++)
        normalizeName(info.cast[i]);

    // Step 3: Merge updates
    for (int i = 0; i < 5; i++)
    {
        if (strlen(info.cast[i]) == 0 || strcasecmp(info.cast[i], "done") == 0)
            continue;

        int found = -1;
        for (int j = 0; j < cast_count; j++)
        {
            if (strcasecmp(existing[j].name, info.cast[i]) == 0 || similarName(existing[j].name, info.cast[i]))
            {
                found = j;
                break;
            }
        }

        if (found == -1)
        {
            // New cast
            strcpy(existing[cast_count].name, info.cast[i]);
            existing[cast_count].count_shows = 0;
            existing[cast_count].count_movies = 1;
            strcpy(existing[cast_count].movies[0], info.name);
            existing[cast_count].count_awards = 0;

            for (int j = 0; j < info.count_awards; j++)
            {
                if (strcasecmp(info.receipant[j], info.cast[i]) == 0)
                {
                    strcpy(existing[cast_count].awards[existing[cast_count].count_awards], info.awards[j]);
                    strcpy(existing[cast_count].awards_year[existing[cast_count].count_awards], info.year[j]);
                    existing[cast_count].count_awards++;
                }
            }
            cast_count++;
        }
        else
        {
            // Existing cast — merge movies
            int exists = 0;
            for (int k = 0; k < existing[found].count_movies; k++)
                if (strcasecmp(existing[found].movies[k], info.name) == 0)
                    exists = 1;

            if (!exists)
                strcpy(existing[found].movies[existing[found].count_movies++], info.name);

            // Merge awards
            for (int j = 0; j < info.count_awards; j++)
            {
                if (strcasecmp(info.receipant[j], info.cast[i]) == 0)
                {
                    int dup = 0;
                    for (int k = 0; k < existing[found].count_awards; k++)
                    {
                        if (strcasecmp(existing[found].awards[k], info.awards[j]) == 0 &&
                            strcasecmp(existing[found].awards_year[k], info.year[j]) == 0)
                        {
                            dup = 1;
                            break;
                        }
                    }
                    if (!dup)
                    {
                        strcpy(existing[found].awards[existing[found].count_awards], info.awards[j]);
                        strcpy(existing[found].awards_year[existing[found].count_awards], info.year[j]);
                        existing[found].count_awards++;
                    }
                }
            }
        }
    }

    // Step 4: Write clean file
    fp = fopen("cast_info.txt", "w");
    for (int i = 0; i < cast_count; i++)
    {
        fprintf(fp, "------------ Cast Details ------------\n");
        fprintf(fp, "Cast Name: %s\n", existing[i].name);
        fprintf(fp, "No. TV Shows : %d\n", existing[i].count_shows);
        fprintf(fp, "TV Shows:\n");
        for (int j = 0; j < existing[i].count_shows; j++)
            fprintf(fp, "  - %s\n", existing[i].shows[j]);

        fprintf(fp, "No. Movies : %d\n", existing[i].count_movies);
        fprintf(fp, "Movies:\n");
        for (int j = 0; j < existing[i].count_movies; j++)
            fprintf(fp, "  - %s\n", existing[i].movies[j]);

        fprintf(fp, "No. Awards : %d\n", existing[i].count_awards);
        fprintf(fp, "Awards:\n");
        for (int j = 0; j < existing[i].count_awards; j++)
            fprintf(fp, "  - %s (%s)\n", existing[i].awards[j], existing[i].awards_year[j]);
        fprintf(fp, "--------------------------------------\n\n");
    }
    fclose(fp);

    printf("Cast database updated successfully — no award/show mixups.\n");
}

void list_by_cast()
{
    char search_cast[100], line[256], prod_companies[20][100];
    int prod_count = 0;

    printf("Enter cast member name: ");
    fgets(search_cast, sizeof(search_cast), stdin);
    search_cast[strcspn(search_cast, "\n")] = '\0';

    FILE *fp;
    char filename[2][20] = {"tv_shows.txt", "Movies.txt"};

    for (int f = 0; f < 2; f++)
    {
        fp = fopen(filename[f], "r");
        if (fp == NULL)
            continue;

        char current_name[100] = "";
        int found_cast = 0;

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Name:", 5) == 0)
            {
                sscanf(line, "Name: %[^\n]", current_name);
            }
            else if (_strnicmp(line, "Cast:", 5) == 0)
            {
                char lower_line[256], lower_cast[100];
                int i;

                for (i = 0; line[i] && i < 255; i++)
                    lower_line[i] = tolower(line[i]);
                lower_line[i] = '\0';

                for (i = 0; search_cast[i] && i < 99; i++)
                    lower_cast[i] = tolower(search_cast[i]);
                lower_cast[i] = '\0';

                if (strstr(lower_line, lower_cast) != NULL)
                {
                    found_cast = 1;
                }
            }

            if (strncasecmp(line, "Production Company:", 19) == 0 && found_cast)
            {
                char temp_prod[100];
                sscanf(line, "Production Company: %[^\n]", temp_prod);

                int duplicate = 0;
                for (int i = 0; i < prod_count; i++)
                {
                    if (strcasecmp(temp_prod, prod_companies[i]) == 0)
                    {
                        duplicate = 1;
                        break;
                    }
                }

                if (!duplicate && prod_count < 20)
                {
                    strcpy(prod_companies[prod_count], temp_prod);
                    prod_count++;
                }
                found_cast = 0;
            }
        }
        fclose(fp);
    }

    if (prod_count == 0)
    {
        printf("\nNo entries found for cast: %s\n", search_cast);
        return;
    }

    printf("\n%s has worked with the following production companies:\n", search_cast);
    for (int i = 0; i < prod_count; i++)
        printf("%d. %s\n", i + 1, prod_companies[i]);

    printf("\nListing all TV shows and Movies from these production companies:\n");

    for (int f = 0; f < 2; f++)
    {
        fp = fopen(filename[f], "r");
        if (fp == NULL)
            continue;

        char current_name[100] = "";
        char current_prod[100] = "";

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Name:", 5) == 0)
            {
                sscanf(line, "Name: %[^\n]", current_name);
            }
            else if (strncasecmp(line, "Production Company:", 19) == 0)
            {
                sscanf(line, "Production Company: %[^\n]", current_prod);
                for (int i = 0; i < prod_count; i++)
                {
                    if (strcasecmp(current_prod, prod_companies[i]) == 0)
                    {
                        printf("- %s (%s)\n", current_name, current_prod);
                    }
                }
            }
        }
        fclose(fp);
    }
}

void displayWorksByCast()
{
    char cast_name[100], line[256];
    printf("Enter Cast Name: ");
    fgets(cast_name, sizeof(cast_name), stdin);
    cast_name[strcspn(cast_name, "\n")] = '\0';

    struct
    {
        char title[100];
        int year;
        char type[10]; // TV or Movie
    } works[100];
    int count = 0;

    char *files[2] = {"tv_shows.txt", "Movies.txt"};

    for (int f = 0; f < 2; f++)
    {
        FILE *fp = fopen(files[f], "r");
        if (!fp)
            continue;

        char current_name[100] = "";
        int current_year = 0;
        int cast_found = 0;

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Name:", 5) == 0)
            {
                sscanf(line, "Name: %[^\n]", current_name);
                cast_found = 0; // reset
            }
            else if (strncasecmp(line, "Original Release Date:", 22) == 0)
            {
                // For TV shows
                sscanf(line, "Original Release Date: %*d-%*d-%d", &current_year);
            }
            else if (strncasecmp(line, "Release Date:", 13) == 0)
            {
                // For Movies
                sscanf(line, "Release Date: %*d-%*d-%d", &current_year);
            }
            else if (strncasecmp(line, "Cast:", 5) == 0)
            {
                char lower_line[256], lower_cast[100];
                for (int i = 0; line[i]; i++)
                    lower_line[i] = tolower(line[i]);
                lower_line[strlen(line)] = '\0';
                for (int i = 0; cast_name[i]; i++)
                    lower_cast[i] = tolower(cast_name[i]);
                lower_cast[strlen(cast_name)] = '\0';

                if (strstr(lower_line, lower_cast))
                    cast_found = 1;
            }
            else if (strstr(line, "-------------------------------------") && cast_found)
            {
                strcpy(works[count].title, current_name);
                works[count].year = current_year;
                strcpy(works[count].type, (f == 0) ? "TV" : "Movie");
                count++;
                cast_found = 0;
            }
        }
        fclose(fp);
    }

    if (count == 0)
    {
        printf("\nNo TV Shows or Movies found for cast '%s'\n", cast_name);
        return;
    }

    // Sort descending by year
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (works[i].year < works[j].year)
            {
                typeof(works[0]) temp = works[i];
                works[i] = works[j];
                works[j] = temp;
            }
        }
    }

    printf("\n------------ Works of %s ------------\n", cast_name);
    for (int i = 0; i < count; i++)
        printf("%d. %s (%s, %d)\n", i + 1, works[i].title, works[i].type, works[i].year);
}

void search_awards()
{
    char search_name[100];
    printf("Enter TV Show or Movie name: ");
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = '\0';

    int f;
    printf("\nType of content:\n");
    printf("1. TV Show\n");
    printf("2. Movie\n");
    printf("Enter your choice: ");
    scanf("%d", &f);
    getchar();

    FILE *fp;
    if (f == 1)
        fp = fopen("tv_shows.txt", "r");
    else
        fp = fopen("Movies.txt", "r");

    if (fp == NULL)
    {
        printf("Error: Unable to open file.\n");
        return;
    }

    char line[256];
    char current_name[100] = "";
    int found_show = 0, found_award = 0;

    // Convert input to lowercase
    char lower_search[100];
    for (int i = 0; search_name[i]; i++)
        lower_search[i] = tolower(search_name[i]);
    lower_search[strlen(search_name)] = '\0';

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "Name:", 5) == 0)
        {
            sscanf(line, "Name: %[^\n]", current_name);

            char lower_name[100];
            for (int i = 0; current_name[i]; i++)
                lower_name[i] = tolower(current_name[i]);
            lower_name[strlen(current_name)] = '\0';

            if (strcmp(lower_name, lower_search) == 0)
            {
                found_show = 1;
                printf("\nAwards for '%s' (%s):\n",
                       current_name, (f == 1) ? "TV Show" : "Movie");
            }
            else
            {
                found_show = 0;
            }
        }

        // Print award lines inside matching block
        else if (found_show && (strstr(line, "—") || strstr(line, "-")) && strstr(line, "("))
        {
            char award_name[100], award_year[10], recipient[100];
            int matched = 0;

            if (sscanf(line, " %[^'(] (%[^)]) — %[^\n]", award_name, award_year, recipient) == 3)
                matched = 1;
            else if (sscanf(line, " %[^'(] (%[^)]) - %[^\n]", award_name, award_year, recipient) == 3)
                matched = 1;

            if (matched)
            {
                found_award = 1;
                printf(" %s (%s) - %s\n", award_name, award_year, recipient);
            }
        }

        // End of section
        else if (found_show && strstr(line, "-------------------------------------"))
        {
            break;
        }
    }

    if (!found_award)
    {
        printf("\nNo awards found for '%s' in %s.\n",
               search_name, (f == 1) ? "TV Shows" : "Movies");
    }

    fclose(fp);
}

void listAwardsByCast()
{
    char cast_name[100];
    printf("Enter Cast Name: ");
    fgets(cast_name, sizeof(cast_name), stdin);
    cast_name[strcspn(cast_name, "\n")] = '\0';

    struct
    {
        char award[100];
        char title[100];
        int year;
        char type[10]; // TV or Movie
    } awards[100];
    int count = 0;

    char *files[2] = {"tv_shows.txt", "Movies.txt"};

    for (int f = 0; f < 2; f++)
    {
        FILE *fp = fopen(files[f], "r");
        if (!fp)
            continue;

        char line[256];
        char current_title[100] = "";
        int in_award_block = 0;

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Name:", 5) == 0)
            {
                sscanf(line, "Name: %[^\n]", current_title);
                in_award_block = 0;
            }
            else if (strncasecmp(line, "Awards:", 7) == 0)
            {
                in_award_block = 1;
            }
            else if (in_award_block && (strstr(line, "—") || strstr(line, "-")) && strstr(line, "("))
            {
                char award_name[100], award_year[10], recipient[100];
                int matched = 0;

                if (sscanf(line, " %[^'(] (%[^)]) — %[^\n]", award_name, award_year, recipient) == 3)
                    matched = 1;
                else if (sscanf(line, " %[^'(] (%[^)]) - %[^\n]", award_name, award_year, recipient) == 3)
                    matched = 1;

                if (!matched)
                    continue;

                // Trim newline/spaces
                award_name[strcspn(award_name, "\n")] = '\0';
                recipient[strcspn(recipient, "\n")] = '\0';

                // Convert to lowercase for comparison
                char rec_lower[100], cast_lower[100];
                for (int i = 0; recipient[i]; i++)
                    rec_lower[i] = tolower(recipient[i]);
                rec_lower[strlen(recipient)] = '\0';
                for (int i = 0; cast_name[i]; i++)
                    cast_lower[i] = tolower(cast_name[i]);
                cast_lower[strlen(cast_name)] = '\0';

                // Match recipient name only
                if (strstr(rec_lower, cast_lower))
                {
                    // Avoid duplicates
                    int duplicate = 0;
                    for (int k = 0; k < count; k++)
                    {
                        if (strcmp(awards[k].award, award_name) == 0 &&
                            strcmp(awards[k].title, current_title) == 0 &&
                            awards[k].year == atoi(award_year))
                        {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (duplicate)
                        continue;

                    strcpy(awards[count].award, award_name);
                    strcpy(awards[count].title, current_title);
                    awards[count].year = atoi(award_year);
                    strcpy(awards[count].type, (f == 0) ? "TV Show" : "Movie");
                    count++;
                }
            }
            else if (strstr(line, "-------------------------------------"))
            {
                in_award_block = 0;
            }
        }
        fclose(fp);
    }
    // Not Found For Recipant
    if (count == 0)
    {
        printf("\nNo awards or nominations found for cast '%s'\n", cast_name);
        return;
    }

    // Sort descending by year
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (awards[i].year < awards[j].year)
            {
                typeof(awards[0]) temp = awards[i];
                awards[i] = awards[j];
                awards[j] = temp;
            }
        }
    }

    // Display formatted output
    printf("\n------------ Awards of %s ------------\n", cast_name);

    int has_tv = 0, has_movie = 0;

    // Check existence of both types
    for (int i = 0; i < count; i++)
    {
        if (strcmp(awards[i].type, "TV Show") == 0)
            has_tv = 1;
        else if (strcmp(awards[i].type, "Movie") == 0)
            has_movie = 1;
    }

    // TV Show Awards
    if (has_tv)
    {
        printf("\n------------ TV Show Awards ------------\n");
        for (int i = 0; i < count; i++)
        {
            if (strcmp(awards[i].type, "TV Show") == 0)
            {
                printf("\n%s (%d)\n", awards[i].award, awards[i].year);
                printf("From: %s\n", awards[i].title);
            }
        }
    }

    // Movie Awards
    if (has_movie)
    {
        printf("\n------------ Movie Awards ------------\n");
        for (int i = 0; i < count; i++)
        {
            if (strcmp(awards[i].type, "Movie") == 0)
            {
                printf("\n%s (%d)\n", awards[i].award, awards[i].year);
                printf("From: %s\n", awards[i].title);
            }
        }
    }

    printf("\n----------------------------------------\n");
}

void ListWorksByCast()
{
    char cast_name[100];
    char line[256];
    FILE *fp;

    printf("Enter Cast Name: ");
    fgets(cast_name, sizeof(cast_name), stdin);
    cast_name[strcspn(cast_name, "\n")] = '\0';

    char filename[2][20] = {"tv_shows.txt", "Movies.txt"};
    char *type_label[2] = {"TV Shows", "Movies"};

    char lower_cast[100];
    for (int i = 0; cast_name[i]; i++)
        lower_cast[i] = tolower(cast_name[i]);
    lower_cast[strlen(cast_name)] = '\0';

    printf("\n---------- Works of %s ----------\n", cast_name);

    for (int f = 0; f < 2; f++)
    {
        fp = fopen(filename[f], "r");
        if (fp == NULL)
            continue;

        char curr_contname[100] = "";
        int found_any = 0;

        printf("%s:\n", type_label[f]);

        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "Name:", 5) == 0)
            {
                sscanf(line, "Name: %[^\n]", curr_contname);
            }
            else if (_strnicmp(line, "Cast:", 5) == 0)
            {
                char lower_line[256];
                for (int i = 0; line[i]; i++)
                    lower_line[i] = tolower(line[i]);
                lower_line[strlen(line)] = '\0';

                if (strstr(lower_line, lower_cast) != NULL)
                {
                    printf("  - %s\n", curr_contname);
                    found_any = 1;
                }
            }
        }

        if (!found_any)
            printf("  (None)\n");

        printf("\n");
        fclose(fp);
    }
}

// USER 
void user_to_file(struct User u) {
    FILE *fp = fopen("users.txt", "a");
    if (!fp) {
        printf("Error: Could not open users.txt\n");
        return;
    }

    if (strcasecmp(u.subs.plan, "silver") == 0)
        u.subs.amount = 199.0;
    else if (strcasecmp(u.subs.plan, "gold") == 0)
        u.subs.amount = 299.0;
    else if (strcasecmp(u.subs.plan, "platinum") == 0)
        u.subs.amount = 399.0;
    else
        u.subs.amount = 0.0; // for trial or unknown plans

    fprintf(fp, "---------- USER DETAILS ----------\n");
    fprintf(fp, "User ID: %d\n", u.id);
    fprintf(fp, "Name: %s\n", u.name);
    fprintf(fp, "Registration Date: %02d-%02d-%04d\n",
            u.reg_day, u.reg_month, u.reg_year);

    fprintf(fp, "\nSUBSCRIPTION DETAILS\n");
    fprintf(fp, "Plan: %s\n", u.subs.plan);
    fprintf(fp, "Start Date: %02d-%02d-%04d\n", 
            u.subs.start_day, u.subs.start_month, u.subs.start_year);
    fprintf(fp, "End Date: %02d-%02d-%04d\n", 
            u.subs.end_day, u.subs.end_month, u.subs.end_year);
    fprintf(fp, "Amount: ₹%.2f\n", u.subs.amount);

    fprintf(fp, "\nPREFERENCES\n");
    if (u.pref_count == 0)
        fprintf(fp, "(None)\n");
    else {
        for (int i = 0; i < u.pref_count; i++)
            fprintf(fp, "%s\n", u.preferences[i]);
    }

    fprintf(fp, "\nVIEWING HISTORY\n");
    if (u.view_count == 0)
        fprintf(fp, "(No shows or movies matched preferences)\n");
    else {
        for (int i = 0; i < u.view_count; i++) {
            fprintf(fp, "%s | %s | Hours Watched: %.1f\n",
                    u.views[i].title,
                    u.views[i].type,
                    u.views[i].hours_watched);
        }
    }

    fprintf(fp, "\nACTIVE SUBSCRIPTIONS\n");
    if (u.show_count == 0)
        fprintf(fp, "(None)\n");
    else {
        for (int i = 0; i < u.show_count; i++)
            fprintf(fp, "%s\n", u.subscribed_shows[i]);
    }

    fprintf(fp, "\nFOLLOWING\n");
    if (u.follow_count == 0)
        fprintf(fp, "(None)\n");
    else {
        for (int i = 0; i < u.follow_count; i++)
            fprintf(fp, "%s\n", u.following[i]);
    }

    fprintf(fp, "-------------------------------------\n\n");
    fclose(fp);

    printf("User details saved successfully.\n");
}

void find_content_by_preferences(char *filename, char *type, struct User *u) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("\n%s database not found.\n", type);
        return;
    }

    char line[256], title[100], genres[200];
    int match_found = 0;

    printf("\n--- %s matching your preferences ---\n", 
           strcmp(type, "Movie") == 0 ? "Movies" : "TV Shows");

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0)
            sscanf(line, "Name: %[^\n]", title);
        else if (strncasecmp(line, "Genres:", 7) == 0) {
            sscanf(line, "Genres: %[^\n]", genres);

            char genres_lower[256];
            strcpy(genres_lower, genres);
            for (int j = 0; genres_lower[j]; j++) genres_lower[j] = tolower(genres_lower[j]);

            for (int i = 0; i < u->pref_count; i++) {
                char pref_lower[50];
                strcpy(pref_lower, u->preferences[i]);
                for (int j = 0; pref_lower[j]; j++) pref_lower[j] = tolower(pref_lower[j]);

                if (strstr(genres_lower, pref_lower)) {
                    strcpy(u->views[u->view_count].title, title);
                    strcpy(u->views[u->view_count].type, type);
                    u->views[u->view_count].hours_watched = 0.0;
                    u->view_count++;
                    printf("  - %s (%s)\n", title, genres);
                    match_found = 1;
                    break;
                }
            }
        }
    }

    if (!match_found)
        printf("  (No %s match your preferences.)\n", type);

    fclose(fp);
}

void add_user() {
    struct User u = {0};

    printf("Enter User ID: ");
    scanf("%d", &u.id);
    getchar();

    printf("Enter Name: ");
    fgets(u.name, sizeof(u.name), stdin);
    u.name[strcspn(u.name, "\n")] = '\0';

    // Registration date
    get_today_date(&u.reg_day, &u.reg_month, &u.reg_year);
    printf("Registration Date: %02d-%02d-%04d\n", u.reg_day, u.reg_month, u.reg_year);

    // Subscription details
    strcpy(u.subs.plan, "silver");
    u.subs.start_day = u.reg_day;
    u.subs.start_month = u.reg_month;
    u.subs.start_year = u.reg_year;
    u.subs.end_day = u.reg_day;
    u.subs.end_month = u.reg_month;
    u.subs.end_year = u.reg_year + 1;
    u.subs.amount = 199.0;

    printf("\nYou have been registered successfully on %02d-%02d-%04d.\n", 
           u.reg_day, u.reg_month, u.reg_year);
    printf("Your trial subscription is valid until %02d-%02d-%04d.\n", 
           u.subs.end_day, u.subs.end_month, u.subs.end_year);

    // Preferences
    printf("\nEnter number of preferences (max 5): ");
    scanf("%d", &u.pref_count);
    getchar();
    if (u.pref_count > 5) u.pref_count = 5;

    for (int i = 0; i < u.pref_count; i++) {
        printf("Preference %d: ", i + 1);
        fgets(u.preferences[i], sizeof(u.preferences[i]), stdin);
        u.preferences[i][strcspn(u.preferences[i], "\n")] = '\0';
    }

    // Follow users
    int x = find_matching_users(u.preferences, u.pref_count);
    if (x == 1) {
        char choice[5];
        printf("\nWould you like to follow any of these users? (yes/no): ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = '\0';
        if (strcasecmp(choice, "yes") == 0)
            printf("Following list updated for '%s'.\n", u.name);
    }

    // Match preferences
    u.view_count = 0;
    find_content_by_preferences("tv_shows.txt", "TV", &u);
    find_content_by_preferences("Movies.txt", "Movie", &u);

    // Subscription input
    printf("\nWould you like to subscribe to any Movies or TV Shows today? (yes/no): ");
    char sub_choice[5];
    fgets(sub_choice, sizeof(sub_choice), stdin);
    sub_choice[strcspn(sub_choice, "\n")] = '\0';

    if (strcasecmp(sub_choice, "yes") == 0) {
        printf("Enter titles to subscribe (type 'done' when finished):\n");
        u.show_count = 0;
        while (u.show_count < 10) {
            char title[100];
            printf("  Title %d: ", u.show_count + 1);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = '\0';
            if (strcasecmp(title, "done") == 0) break;
            strcpy(u.subscribed_shows[u.show_count], title);
            u.show_count++;
        }

        printf("\nActive Subscriptions (%d):\n", u.show_count);
        for (int i = 0; i < u.show_count; i++)
            printf("  - %s\n", u.subscribed_shows[i]);
    } else {
        printf("You can subscribe later from your dashboard.\n");
    }

    // Save user
    user_to_file(u);

    printf("\nWelcome, %s! You are now subscribed on %02d-%02d-%04d.\n", 
           u.name, u.reg_day, u.reg_month, u.reg_year);
    printf("Current Plan: %s (Valid till %02d-%02d-%04d, Rs.%.2f)\n", 
           u.subs.plan, u.subs.end_day, u.subs.end_month, u.subs.end_year, u.subs.amount);
}

int find_matching_users(char prefs[5][50], int pref_count)
{
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf("No existing users found.\n");
        return -1;
    }

    char line[256];
    char current_name[50] = "";
    char user_prefs[5][50];
    int pref_index = 0;
    int has_match = 0;
    int inside_user = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "---------- USER DETAILS ----------", 34) == 0) {
            inside_user = 1;
            pref_index = 0;
            current_name[0] = '\0';
        } 
        else if (inside_user && strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name: %[^\n]", current_name);
        } 
        else if (inside_user && strstr(line, "Preference") && strstr(line, ")")) {
            sscanf(line, "  %*d) %[^\n]", user_prefs[pref_index]);
            pref_index++;
        } 
        else if (inside_user && strstr(line, "-------------------------------------")) {
            for (int i = 0; i < pref_index; i++) {
                for (int j = 0; j < pref_count; j++) {
                    char a[50], b[50];
                    strcpy(a, user_prefs[i]);
                    strcpy(b, prefs[j]);
                    to_lower(a);
                    to_lower(b);
                    if (strcmp(a, b) == 0) {
                        if (!has_match) {
                            printf("\nUsers with similar preferences found:\n");
                            has_match = 1;
                        }
                        printf("\nUser: %s (matched on: %s)\n", current_name, prefs[j]);

                        FILE *subfp = fopen("users.txt", "r");
                        if (subfp) {
                            char subline[256];
                            int in_target = 0, in_sub_section = 0;
                            while (fgets(subline, sizeof(subline), subfp)) {
                                if (strstr(subline, "Name:")) {
                                    char temp_name[100];
                                    sscanf(subline, "Name: %[^\n]", temp_name);
                                    if (strcasecmp(temp_name, current_name) == 0)
                                        in_target = 1;
                                    else
                                        in_target = 0;
                                }
                                else if (in_target && strstr(subline, "Subscribed TV Shows")) {
                                    in_sub_section = 1;
                                    printf("  Subscribed TV Shows:\n");
                                }
                                else if (in_sub_section && strstr(subline, "Following") == 0 && strstr(subline, "-------------------------------------") == 0) {
                                    if (strlen(subline) > 3)
                                        printf("   %s", subline);
                                }
                                else if (in_sub_section && strstr(subline, "-------------------------------------")) {
                                    in_sub_section = 0;
                                    in_target = 0;
                                }
                            }
                            fclose(subfp);
                        }
                    }
                }
            }
            inside_user = 0;
        }
    }

    if (!has_match)
        printf("\nNo users found with matching preferences.\n");
    fclose(fp);
    return has_match;
}

void subscribed_cont() {
    int temp_id;
    char line[256];
    int found_user = 0;
    int in_subscription_section = 0;

    printf("Enter user id: ");
    scanf("%d", &temp_id);
    getchar(); // clear newline

    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) {
        printf("Unable to open 'users.txt'!\n");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, "User ID:", 8) == 0) {
            int id_in_file;
            sscanf(line, "User ID: %d", &id_in_file);

            if (id_in_file == temp_id) {
                found_user = 1;
                printf("\nUser ID: %d found.\n", id_in_file);
            } else {
                found_user = 0;
                in_subscription_section = 0;
            }
        }

        if (found_user && strncmp(line, "ACTIVE SUBSCRIPTIONS", 20) == 0) {
            printf("\n--- Active Subscriptions ---\n");
            in_subscription_section = 1;
            continue;
        }

        if (in_subscription_section) {
            if (strncmp(line, "FOLLOWING", 9) == 0 || strncmp(line, "----------", 10) == 0) {
                in_subscription_section = 0;
                printf("------------------------------\n");
                break;
            }
            // print line if it is not next feild title or end of file//
            if (strlen(line) > 0)
                printf("%s\n", line);
        }
    }

    if (!found_user)
        printf("\nNo user found with ID %d.\n", temp_id);

    fclose(fp);
}

void categorize_user() {
    int target_id;
    printf("Enter User ID: ");
    scanf("%d", &target_id);
    getchar();

    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (fp == NULL || temp == NULL) {
        printf("Error: Could not open file.\n");
        return;
    }

    char line[256];
    int found = 0;
    int start_day = 0, start_month = 0, start_year = 0;
    int end_day = 0, end_month = 0, end_year = 0;
    char current_plan[20] = "";
    char new_plan[20] = "";
    float amount = 0.0;

    while (fgets(line, sizeof(line), fp)) {
        fputs(line, temp);

        if (strncmp(line, "User ID:", 8) == 0) {
            int id_in_file;
            sscanf(line, "User ID: %d", &id_in_file);
            if (id_in_file == target_id) {
                found = 1;
                printf("\nUser ID %d found.\n", id_in_file);
            } else {
                found = 0;
            }
        }

        if (found && strncmp(line, "Start Date:", 11) == 0)
            sscanf(line, "Start Date: %d-%d-%d", &start_day, &start_month, &start_year);

        if (found && strncmp(line, "End Date:", 9) == 0)
            sscanf(line, "End Date: %d-%d-%d", &end_day, &end_month, &end_year);

        if (found && strncmp(line, "Plan:", 5) == 0)
            sscanf(line, "Plan: %s", current_plan);

        if (found && start_year != 0 && end_year != 0) {
            int total_start_months = start_year * 12 + start_month;
            int total_end_months = end_year * 12 + end_month;
            float duration = (total_end_months - total_start_months) / 12.0;
//
            if (duration > 3.0) {
                strcpy(new_plan, "platinum");
                amount = 399.0;
            } else if (duration >= 1.5) {
                strcpy(new_plan, "gold");
                amount = 299.0;
            } else {
                strcpy(new_plan, "silver");
                amount = 199.0;
            }

            if (strcmp(new_plan, current_plan) != 0) {
                printf("Current Plan: %s → Updated Plan: %s (₹%.2f)\n",
                       current_plan, new_plan, amount);
                fprintf(temp, "Plan: %s\n", new_plan);
                fprintf(temp, "Amount: ₹%.2f\n", amount);

                while (fgets(line, sizeof(line), fp)) {
                    if (strncmp(line, "PREFERENCES", 11) == 0) {
                        fputs(line, temp);
                        break;
                    }
                }
            } else {
                printf("Current Plan: %s\n", current_plan);
            }

            start_year = end_year = 0;
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("\nUser details updated successfully in DataBase.\n");
    } else {
        remove("temp.txt");
        printf("\nUser ID %d not found.\n", target_id);
    }
}

void most_viewed_movie() {
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) {
        printf("Error: Could not open users.txt\n");
        return;
    }

    char line[200];
    char movie_titles[200][100];
    int movie_count[200] = {0};
    int total_movies = 0;
    int in_active_section = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';  // remove newline

        if (strstr(line, "ACTIVE SUBSCRIPTIONS")) {
            in_active_section = 1;
            continue;
        }

        if (strstr(line, "-------------------------------------")) {
            in_active_section = 0;
            continue;
        }

        if (in_active_section && strlen(line) > 0 && strcmp(line, "(None)") != 0) {
            for (int i = 0; line[i]; i++)
                line[i] = tolower(line[i]);

            int found = -1;
            for (int j = 0; j < total_movies; j++) {
                if (strcmp(movie_titles[j], line) == 0) {
                    found = j;
                    break;
                }
            }

            if (found == -1) {
                strcpy(movie_titles[total_movies], line);
                movie_count[total_movies] = 1;
                total_movies++;
            } else {
                movie_count[found]++;
            }
        }
    }

    fclose(fp);

    int max_index = -1, max_users = 0;
    for (int i = 0; i < total_movies; i++) {
        if (movie_count[i] > max_users) {
            max_users = movie_count[i];
            max_index = i;
        }
    }

    if (max_index != -1)
        printf("\nMovie viewed by maximum users: %s (%d users)\n",
               movie_titles[max_index], movie_count[max_index]);
    else
        printf("\nNo active movie subscriptions found.\n");
}

// menu section 
void main_menu()
{
    int choice;
    do {
        printf("\n===== MAIN MENU =====\n");
        printf("1. Add/Update Menu\n");
        printf("2. Search Menu\n");
        printf("3. Sort Menu\n");
        printf("4. Other Menu\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            add_update_menu();
            break;
        case 2:
            search_menu();
            break;
        case 3:
            sort_menu();
            break;
        case 4:
            other_menu();
            break;
        case 5:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 5);
}

void add_update_menu()
{
    int choice;
    int input;
    system("cls");
    printf("\n--- Add/Update Menu ---\n");
    printf("1. Add Content\n");
    printf("2. Add User\n");
    printf("3. Add Cast\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        system("cls");
        printf("1.Add Tv Shows.\n");
        printf("2.Add Movies.\n");
        printf("Enter your choice: ");
        scanf("%d", &input);
        getchar();
        if(input == 1)
            input_tvshows();
        else if(input == 2)
            input_movies();
        else 
        {
            printf("Invalid input !\n Returning to Main Menu.");
            main_menu();
        }
        
        break;
    case 2:
        add_user();
        break;
    case 3:
        addCast();
        break;
    default:
        printf("Invalid choice!\n");
    }
}

void search_menu()
{
    int choice;
    system("cls");
    printf("\n========== SEARCH MENU ==========\n");
    printf("1. Display all movies & TV shows performed by a cast (sorted by year)\n");
    printf("2. Display subscribed shows/movies for a user (with expiry warning)\n");
    printf("3. List all TV shows & movies from same production company for a given cast\n");
    printf("4. List all awards for a particular movie (by category)\n");
    printf("5. Back to Main Menu\n");
    printf("=================================\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        displayWorksByCast();
        break;
    case 2:
        subscribed_cont();
        break;
    case 3:
        list_by_cast();
        break;
    case 4:
        search_awards();
        break;
    case 5:
        return;
    default:
        printf("Invalid input!\n");
    }
}

void sort_menu()
{
    int choice;
    system("cls");
    printf("\n========== SORT MENU ==========\n");
    printf("1. List awards won/nominated by cast (movie-wise, TV-wise, descending year)\n");
    printf("2. Categorize subscribed users (Platinum/Gold/Silver)\n");
    // printf("3. Sort TV shows by user viewing time\n");
    printf("4. Back to Main Menu\n");
    printf("================================\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        listAwardsByCast();
        break;
    case 2:
        categorize_user();
        break;
    // case 3:
        // break;
    case 4:
        return;
    default:
        printf("Invalid input!\n");
    }
}

void other_menu()
{
    int choice;
    system("cls");
    printf("\n========== OTHER OPERATIONS MENU ==========\n");
    printf("1. Identify the movie viewed by the maximum number of users in a month\n");
    printf("2. List TV shows and movies for the same casting details\n");
    printf("3. Back to Main Menu\n");
    printf("===========================================\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        most_viewed_movie();
        break;
    case 2:
        ListWorksByCast();
        break;
    case 3:
        return;
    default:
        printf("Invalid input!\n");
    }
}

int main()
{
    main_menu();
    return 0;
}