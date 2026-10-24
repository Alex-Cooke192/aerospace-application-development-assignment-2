#include <gtest/gtest.h>
#include "event-bus/event-bus.h"
#include "event-bus/events.h"

TEST(EventBusTest, DeliversPublishedEvent) {
    EventBus bus;
    int calls = 0;
    float received = -1.0f;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) {
        ++calls;
        received = event.Output_Fuel_Level;
    });
    bus.publish(NewFuelLevelSensorOutput{42.0f});
    EXPECT_EQ(calls, 1);
    EXPECT_FLOAT_EQ(received, 42.0f);
}

TEST(EventBusTest, NotifiesAllSubscribers) {
    EventBus bus;
    int first = 0, second = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto&) { ++first; });
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto&) { ++second; });
    bus.publish(NewFuelLevelSensorOutput{30.0f});
    EXPECT_EQ(first, 1);
    EXPECT_EQ(second, 1);
}

TEST(EventBusTest, KeepsEventTypesSeparate) {
    EventBus bus;
    int fuelCalls = 0, speedCalls = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto&) { ++fuelCalls; });
    bus.subscribe<NewAirSpeedSensorOutput>([&](const auto&) { ++speedCalls; });
    bus.publish(NewFuelLevelSensorOutput{40.0f});
    EXPECT_EQ(fuelCalls, 1);
    EXPECT_EQ(speedCalls, 0);
}

TEST(EventBusTest, PublishWithoutSubscribersDoesNotThrow) {
    EventBus bus;
    EXPECT_NO_THROW(bus.publish(NewFuelLevelSensorOutput{40.0f}));
}

TEST(EventBusTest, RepeatedPublicationsInvokeSubscriberEachTime) {
    EventBus bus;
    int calls = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto&) { ++calls; });
    bus.publish(NewFuelLevelSensorOutput{10.0f});
    bus.publish(NewFuelLevelSensorOutput{11.0f});
    EXPECT_EQ(calls, 2);
}
