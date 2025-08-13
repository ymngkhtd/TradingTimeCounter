#pragma once

#include "IDisplayManager.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include <optional>

namespace TradingTimeCounter {

/**
 * @brief Configuration manager for loading and saving app settings
 * 
 * This class handles reading from and writing to YAML configuration files,
 * providing a clean interface for managing application settings.
 */
class ConfigManager {
public:
    /**
     * @brief Application configuration structure
     */
    struct AppConfig {
        DisplayConfig display;              ///< Display settings
        int timerDurationMinutes = 5;       ///< Timer duration in minutes
        bool autoRestart = true;            ///< Auto-restart timer cycles
        bool startMinimized = false;        ///< Start application minimized
        std::string configVersion = "1.0"; ///< Configuration version
    };

public:
    /**
     * @brief Constructor with config file path
     * @param configFilePath Path to the configuration file
     */
    explicit ConfigManager(const std::string& configFilePath = "config.yaml");

    /**
     * @brief Get current configuration
     * @return Current configuration (loaded or default)
     */
    const AppConfig& getConfig() const;

    /**
     * @brief Save current configuration to file
     * @return true if successful, false otherwise
     */
    bool saveConfig();

    /**
     * @brief Update configuration and optionally save
     * @param config New configuration
     * @param saveToFile Whether to save to file immediately
     * @return true if successful, false otherwise
     */
    bool updateConfig(const AppConfig& config, bool saveToFile = true);

    /**
     * @brief Get default configuration
     * @return Default application configuration
     */
    static AppConfig getDefaultConfig();

    /**
     * @brief Check if config file exists
     * @return true if file exists and is readable
     */
    bool configFileExists() const;

    /**
     * @brief Get config file path
     * @return Path to configuration file
     */
    const std::string& getConfigFilePath() const;

private:
    /**
     * @brief Load configuration from file
     * @return Configuration if successful, std::nullopt if failed
     */
    std::optional<AppConfig> loadConfigFromFile();

    /**
     * @brief Save configuration to file
     * @param config Configuration to save
     * @return true if successful, false otherwise
     */
    bool saveConfigToFile(const AppConfig& config);

    /**
     * @brief Convert YAML node to DisplayConfig
     * @param node YAML node containing display configuration
     * @return DisplayConfig object
     */
    DisplayConfig yamlToDisplayConfig(const YAML::Node& node);

    /**
     * @brief Convert DisplayConfig to YAML node
     * @param config DisplayConfig to convert
     * @return YAML node
     */
    YAML::Node displayConfigToYaml(const DisplayConfig& config);

    /**
     * @brief Validate configuration values
     * @param config Configuration to validate
     * @return true if valid, false otherwise
     */
    bool validateConfig(const AppConfig& config) const;

private:
    std::string m_configFilePath;   ///< Path to configuration file
    AppConfig m_currentConfig;      ///< Current configuration
};

} // namespace TradingTimeCounter
