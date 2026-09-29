#include <iostream>
#include <tgbot/tgbot.h>
#include <vector>
#include <cstdlib>
#include <sstream>

// Define a structure to represent a picture
struct Picture {
    std::string fileId;
    std::vector<std::string> tags;
};

// Define a vector to store the uploaded pictures
std::vector<Picture> pictures;

// Generate a random number within a range
int getRandomNumber(int min, int max) {
    return min + static_cast<int>(rand()) / (static_cast<int>(RAND_MAX / (max - min + 1)) + 1);
}

// Handle incoming messages
void handleMessage(TgBot::Bot& bot, TgBot::Message::Ptr message) {
    if (message->text == "/start") {
        // Send the starter guide
        std::string starterGuide = "Welcome to the Picture Bot! Here are the available commands:\n\n"
            "/upload - Upload a picture (you have to send the picture and the command in one message)\n\n"
            "Also, you can add tags to the picture by just putting them after the /upload command (separated by spaces)\n\n"
            "/getpicture - Get a random picture with tags\n\n"
            "/help - Show the available commands";

        bot.getApi().sendMessage(message->chat->id, starterGuide);
    }
    else if (message->text == "/upload") {
        // Prompt the user to upload a picture
        bot.getApi().sendMessage(message->chat->id, "Please upload a picture and provide tags (separated by spaces) in the same message.");
    }
    else if (message->photo.size() > 0) {
        // Store the uploaded picture and tags
        Picture newPicture;
        newPicture.fileId = message->photo[0]->fileId;

        // Extract tags from the message text
        std::istringstream iss(message->text);
        std::string tag;
        while (iss >> tag) {
            newPicture.tags.push_back(tag);
        }

        pictures.push_back(newPicture);
        bot.getApi().sendMessage(message->chat->id, "Picture uploaded successfully!");
    }
    else if (message->text == "/getpicture") {
        // Retrieve a random picture
        if (pictures.empty()) {
            bot.getApi().sendMessage(message->chat->id, "No pictures available.");
            return;
        }

        int randomIndex = getRandomNumber(0, static_cast<int>(pictures.size()) - 1);
        const Picture& randomPicture = pictures[randomIndex];

        // Prepare the tags as a string
        std::string tagsString = "Tags: ";
        for (const std::string& tag : randomPicture.tags) {
            tagsString += "#" + tag + " ";
        }

        // Send the picture with tags to the user
        bot.getApi().sendPhoto(message->chat->id, randomPicture.fileId, tagsString);
    }
    else if (message->text == "/help") {
        // Show the available commands
        std::string helpMessage = "Available commands:\n\n"
            "/upload - Upload a picture (you have to send the picture and the command in one message)\n"
            "/getpicture - Get a random picture with tags\n"
            "/help - Show the available commands";

        bot.getApi().sendMessage(message->chat->id, helpMessage);
    }
}

int main() {
    // Set up the bot
    TgBot::Bot bot("YOUR_API_HERE");

    // Register the message handler
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        handleMessage(bot, message);
        });

    // Start the bot
    try {
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Telegram bot error: " << e.what() << std::endl;
    }

    return 0;
}
