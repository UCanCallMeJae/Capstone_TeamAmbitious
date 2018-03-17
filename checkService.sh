#!/bin/bash
service=bluetooth

if (( $(ps -ef | grep -v grep | grep $service | wc -l) > 0 ))
then
echo "$service is already running, no changes."
else

echo "Service  is stopped, Starting SERVICE"

/etc/init.d/$service start
fi
