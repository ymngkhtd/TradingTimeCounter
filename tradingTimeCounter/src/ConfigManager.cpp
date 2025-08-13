#include "tradingTimeCounter/ConfigManager.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace TradingTimeCounter {

ConfigManager::ConfigManager(const std::string& configFilePath)
    : m_configFilePath(configFilePath)
    , m_currentConfig(getDefaultConfig()) {
    
    // Try to load configuration from file, fall back to defaults if failed
    auto loadedConfig = loadConfigFromFile();
    if (loadedConfig.has_value()) {
        m_currentConfig = loadedConfig.value();
        std::cout << "Configuration loaded from: " << m_configFilePath << std::endl;
    } else {
        std::cout << "Using default configuration" << std::endl;
    }
}

const ConfigManager::AppConfig& ConfigManager::getConfig() const {
    return m_currentConfig;
}

bool ConfigManager::saveConfig() {
    return saveConfigToFile(m_currentConfig);
}

bool ConfigManager::updateConfig(const AppConfig& config, bool saveToFile) {
    if (!validateConfig(config)) {
        std::cerr << "Invalid configuration, cannot update" << std::endl;
        return false;
    }
    
    m_currentConfig = config;
    
    if (saveToFile) {
        return saveConfigToFile(m_currentConfig);
    }
    
    return true;
}

std::optional<ConfigManager::AppConfig> ConfigManager::loadConfigFromFile() {
    try {
        if (!configFileExists()) {
            std::cout << "Config file not found: " << m_configFilePath << std::endl;
            return std::nullopt;
        }

        YAML::Node root = YAML::LoadFile(m_configFilePath);
        
        AppConfig config;
        
        // Load basic settings
        if (root["timerDurationMinutes"]) {
            config.timerDurationMinutes = root["timerDurationMinutes"].as<int>();
        }
        
        if (root["autoRestart"]) {
            config.autoRestart = root["autoRestart"].as<bool>();
        }
        
        if (root["startMinimized"]) {
            config.startMinimized = root["startMinimized"].as<bool>();
        }
        
        if (root["configVersion"]) {
            config.configVersion = root["configVersion"].as<std::string>();
        }
        
        // Load display configuration
        if (root["display"]) {
            config.display = yamlToDisplayConfig(root["display"]);
        }
        
        // Validate loaded configuration
        if (!validateConfig(config)) {
            std::cerr << "Invalid configuration loaded from file" << std::endl;
            return std::nullopt;
        }
        
        std::cout << "Configuration loaded successfully from: " << m_configFilePath << std::endl;
        return config;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
        return std::nullopt;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool ConfigManager::saveConfigToFile(const AppConfig& config) {
    try {
        if (!validateConfig(config)) {
            std::cerr << "Invalid configuration, cannot save" << std::endl;
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Configuration metadata
        emitter << YAML::Key << "configVersion" << YAML::Value << config.configVersion;
        emitter << YAML::Comment("TradingTimeCounter Configuration File");
        
        // Basic settings
        emitter << YAML::Key << "timerDurationMinutes" << YAML::Value << config.timerDurationMinutes;
        emitter << YAML::Key << "autoRestart" << YAML::Value << config.autoRestart;
        emitter << YAML::Key << "startMinimized" << YAML::Value << config.startMinimized;
        
        // Display configuration
        emitter << YAML::Key << "display" << YAML::Value;
        emitter << displayConfigToYaml(config.display);
        
        emitter << YAML::EndMap;
        
        // Create directory if it doesn't exist
        std::filesystem::path configPath(m_configFilePath);
        std::filesystem::path configDir = configPath.parent_path();
        if (!configDir.empty() && !std::filesystem::exists(configDir)) {
            std::filesystem::create_directories(configDir);
        }
        
        // Write to file
        std::ofstream file(m_configFilePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file for writing: " << m_configFilePath << std::endl;
            return false;
        }
        
        file << emitter.c_str();
        file.close();
        
        std::cout << "Configuration saved to: " << m_configFilePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

ConfigManager::AppConfig ConfigManager::getDefaultConfig() {
    AppConfig config;
    
    // Timer settings
    config.timerDurationMinutes = 5;
    config.autoRestart = true;
    config.startMinimized = false;
    config.configVersion = "1.0";
    
    // Display settings - use defaults from DisplayConfig constructor
    config.display = DisplayConfig{};
    
    return config;
}

bool ConfigManager::configFileExists() const {
    return std::filesystem::exists(m_configFilePath) && 
           std::filesystem::is_regular_file(m_configFilePath);
}

const std::string& ConfigManager::getConfigFilePath() const {
    return m_configFilePath;
}

DisplayConfig ConfigManager::yamlToDisplayConfig(const YAML::Node& node) {
    DisplayConfig config;
    
    // Font settings
    if (node["font"]) {
        const auto& fontNode = node["font"];
        if (fontNode["family"]) config.fontFamily = fontNode["family"].as<std::string>();
        if (fontNode["size"]) config.fontSize = fontNode["size"].as<int>();
        if (fontNode["bold"]) config.isBold = fontNode["bold"].as<bool>();
    }
    
    // Colors
    if (node["colors"]) {
        const auto& colorsNode = node["colors"];
        if (colorsNode["text"]) {
            const auto& textColor = colorsNode["text"];
            if (textColor["r"]) config.textColor.r = textColor["r"].as<int>();
            if (textColor["g"]) config.textColor.g = textColor["g"].as<int>();
            if (textColor["b"]) config.textColor.b = textColor["b"].as<int>();
        }
        if (colorsNode["background"]) {
            const auto& bgColor = colorsNode["background"];
            if (bgColor["r"]) config.backgroundColor.r = bgColor["r"].as<int>();
            if (bgColor["g"]) config.backgroundColor.g = bgColor["g"].as<int>();
            if (bgColor["b"]) config.backgroundColor.b = bgColor["b"].as<int>();
        }
    }
    
    // Window settings
    if (node["window"]) {
        const auto& windowNode = node["window"];
        if (windowNode["width"]) config.windowWidth = windowNode["width"].as<int>();
        if (windowNode["height"]) config.windowHeight = windowNode["height"].as<int>();
        if (windowNode["opacity"]) config.opacity = windowNode["opacity"].as<int>();
    }
    
    // Position settings
    if (node["position"]) {
        const auto& posNode = node["position"];
        if (posNode["x"]) config.positionX = posNode["x"].as<int>();
        if (posNode["y"]) config.positionY = posNode["y"].as<int>();
        if (posNode["draggable"]) config.isDraggable = posNode["draggable"].as<bool>();
        if (posNode["locked"]) config.isLocked = posNode["locked"].as<bool>();
    }
    
    return config;
}

YAML::Node ConfigManager::displayConfigToYaml(const DisplayConfig& config) {
    YAML::Node node;
    
    // Font settings
    node["font"]["family"] = config.fontFamily;
    node["font"]["size"] = config.fontSize;
    node["font"]["bold"] = config.isBold;
    
    // Colors
    node["colors"]["text"]["r"] = config.textColor.r;
    node["colors"]["text"]["g"] = config.textColor.g;
    node["colors"]["text"]["b"] = config.textColor.b;
    
    node["colors"]["background"]["r"] = config.backgroundColor.r;
    node["colors"]["background"]["g"] = config.backgroundColor.g;
    node["colors"]["background"]["b"] = config.backgroundColor.b;
    
    // Window settings
    node["window"]["width"] = config.windowWidth;
    node["window"]["height"] = config.windowHeight;
    node["window"]["opacity"] = config.opacity;
    
    // Position settings
    node["position"]["x"] = config.positionX;
    node["position"]["y"] = config.positionY;
    node["position"]["draggable"] = config.isDraggable;
    node["position"]["locked"] = config.isLocked;
    
    return node;
}

bool ConfigManager::validateConfig(const AppConfig& config) const {
    // Validate timer duration (1-60 minutes)
    if (config.timerDurationMinutes < 1 || config.timerDurationMinutes > 60) {
        std::cerr << "Invalid timer duration: " << config.timerDurationMinutes << " (must be 1-60)" << std::endl;
        return false;
    }
    
    // Validate font size (8-72)
    if (config.display.fontSize < 8 || config.display.fontSize > 72) {
        std::cerr << "Invalid font size: " << config.display.fontSize << " (must be 8-72)" << std::endl;
        return false;
    }
    
    // Validate color values (0-255)
    auto validateColor = [](const DisplayConfig::Color& color, const std::string& name) {
        if (color.r < 0 || color.r > 255 || color.g < 0 || color.g > 255 || color.b < 0 || color.b > 255) {
            std::cerr << "Invalid color values for " << name << ": (" 
                      << color.r << "," << color.g << "," << color.b << ")" << std::endl;
            return false;
        }
        return true;
    };
    
    if (!validateColor(config.display.textColor, "text color")) return false;
    if (!validateColor(config.display.backgroundColor, "background color")) return false;
    
    // Validate opacity (0-255)
    if (config.display.opacity < 0 || config.display.opacity > 255) {
        std::cerr << "Invalid opacity: " << config.display.opacity << " (must be 0-255)" << std::endl;
        return false;
    }
    
    // Validate window size (minimum 50x20)
    if (config.display.windowWidth < 50 || config.display.windowHeight < 20) {
        std::cerr << "Invalid window size: " << config.display.windowWidth 
                  << "x" << config.display.windowHeight << " (minimum 50x20)" << std::endl;
        return false;
    }
    
    return true;
}

} // namespace TradingTimeCounter
