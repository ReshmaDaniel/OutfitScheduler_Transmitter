<?php
//php script for sending the user input extracted from the UI to the database
$servername = "ep-old-surf-a24mumke.eu-central-1.aws.neon.tech";
$dbname = "outfitdb";
$username = "outfitdb_owner";
$password = "nOwZ9WT2pdvK";
$port = "5432"; // Default PostgreSQL port
$endpoint_id = "ep-old-surf-a24mumke";

// Retrieve the GET data from the URL query parameters
$getParams = $_GET;

// Debug: Log the content of $getParams to error log
error_log('Received GET data:');
error_log(print_r($getParams, true));

// Ensure all required fields are present
$requiredFields = ['season', 'material','specific_type', 'outfit_type', 'main', 'colour'];
foreach ($requiredFields as $field) {
    if (!isset($getParams[$field])) {
        echo json_encode(['error' => 'Missing required fields']);
        exit;
    }
}

// Assign GET data to variables

$season = $getParams['season'];
$material = $getParams['material'];
$specific_type = $getParams['specific_type'];
$outfit_type = $getParams['outfit_type'];
$main = $getParams['main'];
$colour = $getParams['colour'];



// Connect to PostgreSQL database on Neon
$dbconn = pg_connect("host=$servername dbname=$dbname user=$username password=$password port=$port sslmode=require options=endpoint=ep-old-surf-a24mumke")
    or die('Could not connect: ' . pg_last_error());

// Insert wardrobe data into WARDROBE table
$query = 'INSERT INTO user2 (SEASON, MATERIAL, SPECIFIC_TYPE, OUTFIT_TYPE,MAIN,COLOUR) VALUES ($1, $2, $3, $4, $5, $6) RETURNING *';
$params = array($season, $material, $specific_type, $outfit_type, $main, $colour);

$result = pg_query_params($dbconn, $query, $params);

if ($result) {
    // Insertion successful, fetch the inserted row
    $insertedRow = pg_fetch_assoc($result);
    echo json_encode(['message' => 'Row inserted successfully', 'row' => $insertedRow]);
} else {
    echo json_encode(['error' => 'Failed to insert row: ' . pg_last_error()]);
}

// Close database connection
pg_close($dbconn);
?>
