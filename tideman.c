#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_cycle(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Check name, update ranks[i] to candidate's index.
    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcmp(name, candidates[i]))
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Map ranks to preferences matrix.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair_count = 0;

    // If current pair being examined isn't a tie, compare
    // them head to head.  Winner has more votes.
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] != preferences[j][i])
            {
                pairs[pair_count].winner = (preferences[i][j] > preferences[j][i]) ? i : j;
                pairs[pair_count].loser = (preferences[i][j] > preferences[j][i]) ? j : i;
                pair_count++;
            }
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Temporary variable for bubbling.
    pair tmp_pair;
    int pref1, pref2;

    // Bubble sort.
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = i + 1; j < pair_count; j++)
        {
            // Consolidating long-windedness.
            if (preferences[i][j] > preferences[j][i])
            {
                pref1 = preferences[i][j] - preferences[j][i];
                pref2 = preferences[i][j + 1] - preferences[j + 1][i];
            }
            else
            {
                pref1 = preferences[j][i] - preferences[i][j];
                pref2 = preferences[j + 1][i] - preferences[i][j + 1];
            }

            // The actual switch.
            if (pref2 > pref1)
            {
                tmp_pair = pairs[j];
                pairs[j] = pairs[i];
                pairs[i] = tmp_pair;
            }
        }
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Citing help received from CS50 Discord users...
    // robin9650: studied her pdf tideman-lock-pairs-tough-process-1
    
    // TheElephant: studied a lock_pairs diagram he received from another,
    // also, he cleared up a paradigm I held, improperly, keeping me from a solution.
    // He also helped me to remember how stacks worked, where I realized it needs a 
    // final return value after all other recursive calls to a function have resolved.
    
    // Crash: clued me in to the fact that my recursive algorithm couldn't
    // solve all possible scenarios, even if it satisfied check50.
    
    // None of these people gave me code, just pushes in the right direction.

    // Loop through pairs array, feed to is_cycle().
    for (int i = 0; i < pair_count; i++)
    {
        if (!is_cycle(pairs[i].winner, pairs[i].loser))
        {
            // If no cycle, lock.
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (!locked[j][i])
            {
                printf("%s\n", candidates[j]);
                break;
            }
        }
    }

    return;
}

bool is_cycle(int winner, int loser)
{
    // If original loser ever beats new winner.
    if (locked[loser][winner])
    {
        return true;
    }
    
    // Loop through what's been locked.
    for (int i = 0; i < candidate_count; i++)
    {
        // If the winner is ever a loser,
        if (locked[i][winner])
        {
            // check i for cycle.
            if (is_cycle(i, loser))
            {
                return true;
            }
        }
    }

    return false;
}
