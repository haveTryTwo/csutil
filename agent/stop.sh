#!/bin/sh

ps aux | grep agent_web_server | grep -v grep | grep -v vim | awk -F' ' '{print $2}' | xargs kill -9
