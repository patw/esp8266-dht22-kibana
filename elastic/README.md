Run this in your Kibana dev tools:

This one adds the ingest pipeline to add @timestamp to the temp/humidity docs

PUT _ingest/pipeline/autots
{"description":"add @timestamp","processors":[{"set":{"field":"@timestamp","value":"{{_ingest.timestamp}}"}}]}

This one creates the index and mapping with the proper date format for the ingest pipeline timestamp

PUT temperature/sensor/_mapping
{
    "properties": {
      "@timestamp": {
        "type": "date",
        "format": "EEE MMM dd HH:mm:ss zzz yyyy"
      },
      "humidity": {
        "type": "float"
      },
      "location": {
        "type": "text",
        "fields": {
          "keyword": {
            "type": "keyword",
            "ignore_above": 256
          }
        }
      },
      "temp_c": {
        "type": "float"
      },
      "temp_f": {
        "type": "float"
      }
    }
  }
}
