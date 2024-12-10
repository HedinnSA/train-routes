#include <stdio.h>      // Includes standard input/output library for file I/O and printing, open the CSV file
#include <stdlib.h>     // Includes standard library for memory management and other utilities
#include <string.h>     // Includes string manipulation functions like strcpy and strcmp
#include <limits.h>     // Defines INT_MAX, used to represent an infinite distance in graph traversal
#include <stdbool.h>

#define MAX_STATIONS 500 // Defines the maximum number of stations
#define MAX_NAME_LEN 50  // Defines the maximum length for a station name
#define INF INT_MAX      // Defines a large value (infinity) to represent an unreachable distance

// Structure to store graph edges
typedef struct {
    int time;            // Time to travel between stations
    int distance;        // Distance between stations
    int station_id;      // ID of the destination station
    int co2;             // CO2 emitted for the given route
} Edge;

// Structure to represent a station with adjacency list
typedef struct {
    char name[MAX_NAME_LEN]; // Station name
    Edge edges[MAX_STATIONS]; // Array of edges (connections) to other stations
    int edge_count;           // Number of edges for this station
} Station;

typedef struct {
    int station_id;
    int time;
} Prique_elements;

typedef struct {
    Prique_elements elements[MAX_STATIONS];
    int size;
}Queue;

// Array of stations
Station stations[MAX_STATIONS]; // Holds all station data
int station_count = 0;          // Tracks the number of stations added

void printPath(int end_id, int *prev);
int get_station_id(char *name);
void load_graph(const char *filename);
void add_edge(char *start, char *end, int distance, int time, int co2);
void dijkstra(int start_id, int end_id);
void push(Queue *queue, int station, int time);
void initqueue(Queue *queue);
Prique_elements pop(Queue *queue);
bool is_empty(Queue *queue);

int main(void) {
    const char *filename[] = {"Togruter_2024.csv", "Togruter_2025_IC5.csv", "togruter_2035.csv"}; // File with station and route data
    char start[MAX_NAME_LEN], end[MAX_NAME_LEN];
    const char *Rute[] = {"The route as of 2024", 
                          "The route as of 2027 with IC5 trains", 
                          "The route as of 2035 now with the Kattegat connection and IC5 trains"};

    // Prompt for start and end stations once
    printf("Enter start station: ");
    scanf("%s", start);
    printf("Enter destination station: ");
    scanf("%s", end);

    int start_id = get_station_id(start);  // Get ID for start station
    int end_id = get_station_id(end);      // Get ID for end station

    for (int i = 0; i < 3; i++) {          // Loop through all files
        load_graph(filename[i]);           // Load data from file into graph
        printf("\n%s\n", Rute[i]);         // Prints which route it is
        dijkstra(start_id, end_id);        // Find and display shortest path
    }
    printf("\n");

    return 0;
}

// Function to find station ID by name
int get_station_id(char *name) {
    for (int i = 0; i < station_count; i++) {
        if (strcmp(stations[i].name, name) == 0) {
            return i; // If the station exists, return its ID
        }
    }
    // If station not found, add a new one
    strcpy(stations[station_count].name, name); // Copy the name to the new station
    stations[station_count].edge_count = 0;     // Initialize edge count
    return station_count++;                     // Return new station's ID and increment count
}

// Function to load CSV file data
void load_graph(const char *filename) {
    FILE *file = fopen(filename, "r"); // Opens the CSV file for reading
    if (!file) {
        perror("Failed to open file"); // Prints an error if file can't be opened
        exit(1);
    }
    char line[128]; // Buffer to hold each line of the file
    while (fgets(line, sizeof(line), file)) { // Reads each line of the file
        char start[MAX_NAME_LEN], end[MAX_NAME_LEN];
        int time;
        int distance;
        int co2;
        sscanf(line, "%[^;];%[^;];%d;%d;%d", start, end, &distance, &time, &co2); // Parses line fields
        add_edge(start, end, distance, time, co2); // Adds an edge using the parsed data
    }
    fclose(file); // Closes the file after reading
}

// Function to add an edge between two stations
void add_edge(char *start, char *end, int distance, int time, int co2) {    
    int start_id = get_station_id(start); // Get or create ID for start station    
    int end_id = get_station_id(end);     // Get or create ID for end station    

    // Add edge in both directions (undirected graph)
    
    // Using a pointer here avoids copying the Edge structure, allowing direct manipulation 
    // of the data in memory for the current edge at stations[start_id].edges[edge_count].
    Edge *Edge_count_start = &stations[start_id].edges[stations[start_id].edge_count]; 
    Edge_count_start->station_id = end_id; // Efficiently access and modify edge properties
    Edge_count_start->time = time;
    Edge_count_start->distance = distance;
    Edge_count_start->co2 = co2;
    stations[start_id].edge_count++; // Increment edge count for the start station

    // Similarly, using a pointer for the end station avoids redundancy and improves memory efficiency.
    Edge *Edge_count_end = &stations[end_id].edges[stations[end_id].edge_count];
    Edge_count_end->station_id = start_id; // Avoids creating a temporary copy of the Edge struct
    Edge_count_end->time = time;
    Edge_count_end->distance = distance;
    Edge_count_end->co2 = co2;
    stations[end_id].edge_count++; // Increment edge count for the end station
}


// Dijkstra's algorithm for shortest path
void dijkstra(int start_id, int end_id) {
    int visited[MAX_STATIONS], prev[MAX_STATIONS], dist[MAX_STATIONS], tid[MAX_STATIONS], co2[MAX_STATIONS];
    int co2_fast = 6;
    for (int i = 0; i < station_count; i++) {
        dist[i] = INF;     // Initialize distances as infinity
        tid[i] = INF;      // Initialize travel times as infinity
        co2[i] = INF;      // Initialize CO2 emition as infinity
        prev[i] = -1;      // Initialize previous stations as undefined
        visited[i] = 0;    // Mark all stations as unvisited
    }
    dist[start_id] = 0;    // Distance to start station is 0
    tid[start_id] = 0;     // Travel time to start station is 0
    co2[start_id] = 0;     // CO2 to start station is 0
    
    Queue queue;
    initqueue(&queue);
    push(&queue, start_id, 0);

    while(!is_empty(&queue)){
        Prique_elements current = pop(&queue);
        int u = current.station_id;
    
        if(visited[u]) continue;
        visited[u] = 1; // Mark station u as visited

        for (int j = 0; j < stations[u].edge_count; j++) {
            int v = stations[u].edges[j].station_id;
            int alt = dist[u] + stations[u].edges[j].distance;
            int altt = tid[u] + stations[u].edges[j].time;
            int altc = co2[u] + stations[u].edges[j].time;
            if (altt < tid[v]) { // Update time if a faster path is found
                tid[v] = altt;
                dist[v] = alt;
                co2[v] = altc;
                prev[v] = u;
                push(&queue, v, altt);
            }
        }
    }
    

        // Print shortest path
    if (dist[end_id] == INF) {
        printf("No route found from %s to %s\n", stations[start_id].name, stations[end_id].name);
        return;
    }
    printf("Shortest route from %s to %s (Distance: %d km)(Time: %d min)(CO2 Emission per/person: %d grams):\n", stations[start_id].name, stations[end_id].name, dist[end_id], tid[end_id], co2[end_id]);

    char user_input[10];
    printf("Do you want the path order of the stations? (yes/no): ");
    scanf("%s", user_input);

    if (strcmp(user_input, "yes") == 0) {
        printPath(end_id, prev);    
        } else {
        printf("Path order will not be displayed.\n");
    }

}

void printPath(int end_id, int *prev) {
    int path[MAX_STATIONS], path_len = 0;
    for (int v = end_id; v != -1; v = prev[v]) {
        path[path_len++] = v;
    }
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%s", stations[path[i]].name);
        if (i > 0) printf(" -> \n");
    }
    printf("\n");
}

void push(Queue *queue, int station, int time){
    int i = queue->size++;

    while(i>0 && queue->elements[(i-1)/2].time > time){
        queue->elements[i] = queue->elements[(i-1)/2];
        i = ((i-1)/2);
    }

    queue->elements[i].station_id = station;
    queue->elements[i].time = time;
}
void initqueue(Queue *queue){
    queue->size = 0;
}
Prique_elements pop(Queue *queue){
    Prique_elements min_el = queue->elements[0];
    Prique_elements max_el = queue->elements[--queue->size];

    int prev, i = 0;

    while((prev = 2*i+1) < queue->size){
        if((prev+1) < queue->size && queue->elements[prev+1].time < queue->elements[prev].time){
            prev++;
        }
        if(max_el.time <= queue->elements[prev].time){
            break;
        }
        queue->elements[i] = queue->elements[prev];
        i = prev;
    }
    queue->elements[i] = max_el;
    return min_el;

}
bool is_empty(Queue *queue){
    return queue->size == 0;
}
