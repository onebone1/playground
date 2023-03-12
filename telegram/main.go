package main

import (
  "os"
  "log"

  "github.com/go-telegram-bot-api/telegram-bot-api"
  "github.com/joho/godotenv"
)

func main() {
  err := godotenv.Load()
  if err != nil {
    log.Panic("Failed to load .env: ", err)
  }
  token := os.Getenv("BotToken")
  bot, err := tgbotapi.NewBotAPI(token)
  if err != nil {
    log.Panic(err)
  }

  bot.Debug = true
  
  log.Printf("Authorized on account %s", bot.Self.UserName)

  u := tgbotapi.NewUpdate(0)
  u.Timeout=60

  updates, err := bot.GetUpdatesChan(u)

  for update := range updates {
    if update.Message == nil {
      continue 
    }

    log.Printf("[%s] %s", update.Message.From.UserName, update.Message.Text)

    msg := tgbotapi.NewMessage(update.Message.Chat.ID, update.Message.Text)
    // msg.ReplyToMessageID = update.Message.MessageID

    bot.Send(msg)
  }
}
