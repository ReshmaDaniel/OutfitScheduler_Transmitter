#include <Arduino.h>
#include <ESPUI.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <vector>
#include <espnow.h>

//define wifi setup details and all other required variables and functions 

const char* ssid = "Reshma’s iPhone";
const char* password = "Rechu@99"; //enter actual details here

//For connecting to the server to send and receive information from the hosted database 
// const char* server = "172.20.10.3";  
// String uri = "http://172.20.10.3/add_cloth.php";
//String data = "";

DHT dht(D3, DHT22);
uint16_t temperatureLabel;
uint16_t humidityLabel;

uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0x9C, 0x00, 0x79};

typedef struct struct_message {
 
    float temp;
    float hum;
 
} struct_message;
 
struct_message DHTReading;
 

// Call back for ESP to ESP communication
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
 
  Serial.print("Last Packet Send Status: ");
 
  if (sendStatus == 0) {
 
    Serial.println("Delivery success");
 
  } else {
 
    Serial.println("Delivery fail");
 
  }
 
}


struct ClothingItem {
    //String userType;
    String season;
    String clothtype;
    String colour;
    String name;
    String material;
};


ClothingItem currentItem;
std::vector<ClothingItem> ITEM = { { "WINTER", "Jacket", "Red", "Casual", "Wool"},
        { "SUMMER", "Dress", "Blue", "Summer Dress", "Cotton"},{"SUMMER", "T-Shirt", "White", "Casual", "Cotton"},
    { "AUTUMN", "Jacket", "Brown", "Casual", "Leather"},
    { "SUMMER", "Blouse", "Pink", "Casual", "Silk"},
    {"WINTER", "Sweater", "Gray", "Casual", "Cashmere"},
    {"SUMMER", "Dress", "Purple", "Dress", "Rayon"},
    {"WINTER", "Shorts", "Blue", "Casual", "Denim"},
    {"SUMMER", "Pants", "Beige", "Casual", "Cotton"},
    {"SPRING", "Tank Top", "Black", "Casual", "Polyester"},
    { "SPRING", "Shorts", "Green", "Casual", "Linen"} };

struct URL{ String url; String year;};
URL shopitem;
std::vector<URL> SHOPITEM={{"Link1","2023"}};

String imageUrl;
String itemtobedeleted;
int itemtobedeleted_int;

uint16_t statusLabel;
uint16_t tabmain;
uint16_t IMG_Data;

//uint16_t userType_tb;
uint16_t season_tb;
uint16_t clothingtype_tb;
uint16_t colour_tb;
uint16_t name_tb;
uint16_t material_tb;
uint16_t uploadB;
uint16_t textbox;
uint16_t disptextbox;
uint16_t displayButton;
uint16_t deleteButton;
uint16_t textboxforyear;

//uint16_t userLabel;
uint16_t seasonLabel;
uint16_t clothtypeLabel;
uint16_t colourLabel;
uint16_t nameLabel;
uint16_t materialLabel;
uint16_t logicLabel;
uint16_t Shop; 
uint16_t uploadshop;

uint16_t purchaseCountLabel;

//void userType_tb_callback(Control* sender, int type);
void season_tb_callback(Control* sender, int type);
void clothingtype_tb_callback(Control* sender, int type);
void colour_tb_callback(Control* sender, int type);
void name_tb_callback(Control* sender, int type);
void material_tb_callback(Control* sender, int type);
void textboxCallback(Control* sender, int type);
void uploadDetails(Control* sender, int type);
void DDtextboxCallback(Control* sender, int type);
void displayItemDetails(Control* sender, int type);
void deleteItem(Control* sender, int type);
void textboxFYCallback(Control* sender, int type);
void uploadshoppeditem(Control* sender, int type);

                            //setup function which sets up the wifi connection, sets up esp-esp transmission, and defines all UI features.

void setup() {
  Serial.begin(115200); 
  dht.begin();

  WiFi.mode(WIFI_STA);
 
 
  if (esp_now_init() != 0) {
 
    Serial.println("Error initializing ESP-NOW");
 
    return;
 
  }
 
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
 

  WiFi.begin(ssid, password);
  Serial.println(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(F("Connecting to WiFi..."));
    ESP.wdtFeed(); 
  }
  Serial.println(F("Connected to WiFi"));
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());

  tabmain = ESPUI.addControl(ControlType::Tab, "", "Main Tab");
  statusLabel = ESPUI.addControl(ControlType::Label, "Status:", "Connected to WiFi", ControlColor::Turquoise, tabmain);
  temperatureLabel = ESPUI.addControl(ControlType::Label, "Temperature", "0.0 °C", ControlColor::Turquoise,tabmain);
  humidityLabel = ESPUI.addControl(ControlType::Label, "Humidity", "0.0 %", ControlColor::Turquoise,tabmain);
   

  IMG_Data = ESPUI.addControl(ControlType::Tab, "", "Enter Details");
  //userType_tb = ESPUI.addControl(ControlType::Text, "Enter user type (e.g., BOY, GIRL)", "", ControlColor::Alizarin, IMG_Data, &userType_tb_callback);
  season_tb = ESPUI.addControl(ControlType::Text, "Enter season type (Summer/Winter) for the uploaded cloth", "", ControlColor::Alizarin, IMG_Data, &season_tb_callback);
  clothingtype_tb = ESPUI.addControl(ControlType::Text, "Enter cloth type (Topwear/Bottomwear) for the uploaded cloth", "", ControlColor::Alizarin, IMG_Data, &clothingtype_tb_callback);
  colour_tb = ESPUI.addControl(ControlType::Text, "Enter the color of the uploaded cloth", "", ControlColor::Alizarin, IMG_Data, &colour_tb_callback);
  name_tb = ESPUI.addControl(ControlType::Text, "Enter an outfit type (Casual(ie top/bottom) or Dress)", "", ControlColor::Alizarin, IMG_Data, &name_tb_callback);
  material_tb = ESPUI.addControl(ControlType::Text, "Enter the material of the uploaded cloth", "", ControlColor::Alizarin, IMG_Data, &material_tb_callback);
  uploadB = ESPUI.addControl(ControlType::Button, "Upload details", "Press to upload details", ControlColor::Peterriver, IMG_Data, &uploadDetails);

  uint16_t deets = ESPUI.addControl(ControlType::Tab, "", "View & Delete");
  //userLabel = ESPUI.addControl(ControlType::Label, "User Type:", "Loading", ControlColor::Alizarin, deets);
  seasonLabel = ESPUI.addControl(ControlType::Label, "Season:", "Loading", ControlColor::Alizarin, deets);
  clothtypeLabel = ESPUI.addControl(ControlType::Label, "Cloth Type:", "Loading", ControlColor::Alizarin, deets);
  colourLabel = ESPUI.addControl(ControlType::Label, "Colour:", "Loading", ControlColor::Alizarin, deets);
  nameLabel = ESPUI.addControl(ControlType::Label, "Name:", "Loading", ControlColor::Alizarin, deets);
  materialLabel = ESPUI.addControl(ControlType::Label, "Material:", "Loading", ControlColor::Alizarin, deets);
  disptextbox = ESPUI.addControl(ControlType::Text, "Enter the Item no you want to view/delete:", "", ControlColor::Peterriver, deets, &DDtextboxCallback);
  displayButton = ESPUI.addControl(ControlType::Button, "Display Item Details", "Display", ControlColor::Peterriver, deets, &displayItemDetails);
  deleteButton = ESPUI.addControl(ControlType::Button, "Delete Item", "Delete", ControlColor::Peterriver, deets, &deleteItem);

  Shop = ESPUI.addControl(ControlType::Tab, "", "Shop");
  logicLabel = ESPUI.addControl(ControlType::Label, "Suggestions to buy", "Loading", ControlColor::Turquoise, Shop);
  textbox = ESPUI.addControl(ControlType::Text, "Enter Image URL", "", ControlColor::Alizarin, Shop, &textboxCallback);
  textboxforyear=ESPUI.addControl(ControlType::Text, "Enter year of purchase", "", ControlColor::Alizarin, Shop, &textboxFYCallback);
  uploadshop=ESPUI.addControl(ControlType::Button, "Upload Item", "Upload", ControlColor::Peterriver, Shop, &uploadshoppeditem);
  purchaseCountLabel = ESPUI.addControl(ControlType::Label, "Yearly Purchase Count", "Loading", ControlColor::Turquoise, Shop);


  ESPUI.begin("ESPUI Control");
}


                                      //call backs with which each feature in the UI call to perform its functions

// void userType_tb_callback(Control* sender, int type) {
//   if (sender->id == userType_tb && type == T_VALUE) {
//     currentItem.userType = sender->value;
//     Serial.print(F("User Type Entered: "));
//     Serial.println(currentItem.userType);
//   }
// }

void season_tb_callback(Control* sender, int type) {
  if (sender->id == season_tb && type == T_VALUE) {
    currentItem.season = sender->value;
    Serial.print("Season Entered: ");
    Serial.println(currentItem.season);
  }
}

void clothingtype_tb_callback(Control* sender, int type) {
  if (sender->id == clothingtype_tb && type == T_VALUE) {
    currentItem.clothtype = sender->value;
    Serial.print("Clothing Type Entered: ");
    Serial.println(currentItem.clothtype);
  }
}

void colour_tb_callback(Control* sender, int type) {
  if (sender->id == colour_tb && type == T_VALUE) {
    currentItem.colour = sender->value;
    Serial.print("Colour Entered: ");
    Serial.println(currentItem.colour);
  }
}

void name_tb_callback(Control* sender, int type) {
  if (sender->id == name_tb && type == T_VALUE) {
    currentItem.name = sender->value;
    Serial.print("Name Entered: ");
    Serial.println(currentItem.name);
  }
}

void material_tb_callback(Control* sender, int type) {
  if (sender->id == material_tb && type == T_VALUE) {
    currentItem.material = sender->value;
    Serial.print("Material Entered: ");
    Serial.println(currentItem.material);
  }
}

void uploadDetails(Control* sender, int type) {
  if (sender->id == uploadB && type == B_UP) {
    ClothingItem newItem = { currentItem.season, currentItem.clothtype, currentItem.colour, currentItem.name, currentItem.material};
    ITEM.push_back(newItem);

    Serial.println("-----------------------------");
    Serial.println("New ITEM added:");
    Serial.println("Season: " + newItem.season);
    Serial.println("Cloth Type: " + newItem.clothtype);
    Serial.println("Colour: " + newItem.colour);
    Serial.println("Name: " + newItem.name);
    Serial.println("Material: " + newItem.material);
    Serial.println("-----------------------------");
  }
}

void textboxCallback(Control* sender, int type) {
  if (sender->id == textbox && type == T_VALUE) {
    //imageUrl = sender->value;
    shopitem.url=sender->value;
    Serial.print("Image URL entered: ");
    Serial.println(shopitem.url);

  }
}

void textboxFYCallback(Control* sender, int type){
  if (sender->id == textboxforyear && type == T_VALUE) {
    shopitem.year=sender->value;
    Serial.print("Year bought: ");
    Serial.println(shopitem.year);
  }

}

void uploadshoppeditem(Control* sender, int type)
{
    if (sender->id == uploadshop && type == B_UP) {
        URL newfit = {shopitem.url,shopitem.year};
        SHOPITEM.push_back(newfit);
        Serial.println("-----------------------------");
        Serial.println("New item bought:");
        Serial.println("URL entered: " + shopitem.url);
        Serial.println("Year bought in: " + shopitem.year);
  }

}

void DDtextboxCallback(Control* sender, int type) {
  if (sender->id == disptextbox && type == T_VALUE) {
    itemtobedeleted = sender->value;
    itemtobedeleted_int = itemtobedeleted.toInt();
    Serial.print("Item number entered: ");
    Serial.println(itemtobedeleted_int);
  }
}

void displayItemDetails(Control* sender, int type) {
  if (sender->id == displayButton && type == B_UP) {
    if (itemtobedeleted_int >= 0 && itemtobedeleted_int < ITEM.size()) {
      ClothingItem item = ITEM[itemtobedeleted_int];
      ESPUI.updateLabel(seasonLabel, item.season);
      ESPUI.updateLabel(clothtypeLabel, item.clothtype);
      ESPUI.updateLabel(colourLabel, item.colour);
      ESPUI.updateLabel(nameLabel, item.name);
      ESPUI.updateLabel(materialLabel, item.material);

      Serial.println("Displaying ITEM:");
      Serial.println("Season: " + item.season);
      Serial.println("Cloth Type: " + item.clothtype);
      Serial.println("Colour: " + item.colour);
      Serial.println("Name: " + item.name);
      Serial.println("Material: " + item.material);
      Serial.println("-----------------------------");
    } else {
      Serial.println("Invalid item number.");
    }
  }
}

void deleteItem(Control* sender, int type) {
  if (sender->id == deleteButton && type == B_UP) {
    if (itemtobedeleted_int >= 0 && itemtobedeleted_int < ITEM.size()) {
      ITEM.erase(ITEM.begin() + itemtobedeleted_int);

      Serial.println("Deleted ITEM at index: " + String(itemtobedeleted_int));
      ESPUI.updateLabel(seasonLabel, "Deleted");
      ESPUI.updateLabel(clothtypeLabel, "Deleted");
      ESPUI.updateLabel(colourLabel, "Deleted");
      ESPUI.updateLabel(nameLabel, "Deleted");
      ESPUI.updateLabel(materialLabel, "Deleted");
    } else {
      Serial.println("Invalid item number.");
    }
  }
}

                  //loop function to update certain labels continuously as well as transmit DHT22 values continuously to the connected esp

void loop() {
 
  DHTReading.temp = dht.readTemperature();
  DHTReading.hum = dht.readHumidity();
 
  if (isnan(DHTReading.temp) || isnan(DHTReading.hum)) {
 
    Serial.println("Failed to read from DHT sensor!");
 
    return;
 
  }
 
  esp_now_send(broadcastAddress, (uint8_t *) &DHTReading, sizeof(DHTReading));
 
    ESPUI.updateControlValue(temperatureLabel, String(DHTReading.temp) + " °C");
    ESPUI.updateControlValue(humidityLabel, String(DHTReading.hum) + " %");
 
    // Serial.print("Temperature: ");
    // Serial.print(temperature);
    // Serial.print(" °C");
    // Serial.print("\tHumidity: ");
    // Serial.print(humidity);
    // Serial.println(" %");

    int seasonCounts[4] = {0, 0, 0, 0}; 
    const char* seasonNames[4] = {"SUMMER", "WINTER", "AUTUMN", "SPRING"};

    for (size_t i = 0; i < ITEM.size(); ++i) {
        if (ITEM[i].season == "SUMMER") {
            seasonCounts[0]++;
        } else if (ITEM[i].season == "WINTER") {
            seasonCounts[1]++;
        } else if (ITEM[i].season == "AUTUMN") {
            seasonCounts[2]++;
        } else if (ITEM[i].season == "SPRING") {
            seasonCounts[3]++;
        }
    }
    char suggestions[512];
    suggestions[0] = '\0';

    for (int i = 0; i < 4; ++i) {
        if (seasonCounts[i] < 2) {
            snprintf(suggestions + strlen(suggestions), sizeof(suggestions) - strlen(suggestions),
                     "You have less than two outfit items for %s.\n", seasonNames[i]);
        }
        if (seasonCounts[i] > 10) {
            snprintf(suggestions + strlen(suggestions), sizeof(suggestions) - strlen(suggestions),
                     "You have more than 10 outfit items for %s.\n", seasonNames[i]);
        }
    }

    if (strlen(suggestions) > 0) {
        ESPUI.updateControlValue(logicLabel, suggestions);
    }
    else{
      ESPUI.updateControlValue(logicLabel, "Total items for each seson between 2 & 10");
      }

    std::map<String, int> yearCounts;
    for (size_t i = 0; i < SHOPITEM.size(); ++i) {
        yearCounts[SHOPITEM[i].year]++;
    }

    char yearCountBuffer[512];
    yearCountBuffer[0] = '\0'; 

    for (const auto& entry : yearCounts) {
        snprintf(yearCountBuffer + strlen(yearCountBuffer), sizeof(yearCountBuffer) - strlen(yearCountBuffer),
                 "In year %s you have purchased %d clothes.\n", entry.first.c_str(), entry.second);
    }

    ESPUI.updateControlValue(purchaseCountLabel, yearCountBuffer);
  
  
  //Commented code is how data taken via the UI can be send to the database by connecting to the server. It runs stand alone on ESP8266. 
  //It is commented out since ESP8266 does not have the capacity to host UI as well as sent data at the same time.

  // if (WiFi.status() == WL_CONNECTED) {
  //   data = "season=summer&material=silk&specific_type=topwear&outfit_type=casual&main=shirt&colour=pink"; 
  //   String getRequest = "GET " + uri + "?" + data + " HTTP/1.1\r\n" +
  //                       "Host: " + server + "\r\n" +
  //                       "Connection: close\r\n\r\n";

  //   Serial.println(F("Sending request:"));
  //   Serial.println(getRequest);

  //   WiFiClient client;
  //   if (client.connect(server, 80)) {
  //     client.print(getRequest);
  //     unsigned long timeout = millis();
  //     while (!client.available() && millis() - timeout < 5000) {
  //       delay(10); 
  //     }
  //     if (client.available()) {
  //       String response = client.readString();
  //       Serial.println(F("Received response:"));
  //       Serial.println(response);
  //     } else {
  //       Serial.println(F("Error: No response from server"));
  //     }
  //     client.stop();
  //   } else {
  //     Serial.println(F("Error: Failed to connect to server"));
  //   }
  // } else {
  //   Serial.println(F("Error: WiFi disconnected"));
  //   WiFi.begin(ssid, password); 
  // }



  delay(10000);

}
