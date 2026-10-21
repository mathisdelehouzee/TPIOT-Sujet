FROM confluentinc/cp-kafka-connect-base:7.9.10

# Versions des connecteurs figées pour que le TP soit reproductible
RUN confluent-hub install --no-prompt confluentinc/kafka-connect-jdbc:10.9.9
RUN confluent-hub install --no-prompt confluentinc/kafka-connect-mqtt:1.7.10
