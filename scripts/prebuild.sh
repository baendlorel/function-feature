#!/bin/bash

# 多版本预编译脚本
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"

# 记录当前版本
current_nvm=$(nvm current)
echo "Current node version: $current_nvm"

# 清理之前的预编译文件
rm -rf prebuilds/

# 支持的 Node 版本列表
NODE_VERSIONS=(16 17 18 19 20 21 22 23 24)

echo "Building for Node versions: ${NODE_VERSIONS[@]}"

for version in "${NODE_VERSIONS[@]}"; do
  echo "================================"
  echo "Building for Node $version"
  echo "================================"
  
  # 切换到指定版本
  nvm use $version
  if [ $? -ne 0 ]; then
    echo "Warning: Node $version not installed, skipping..."
    continue
  fi
  
  # 清理并重新编译
  npm run clean
  npm run build
  
  # 使用 prebuildify 打包
  npx prebuildify --napi=false --strip
  
  echo "Completed build for Node $version"
done

echo "================================"
echo "All builds completed!"
echo "Restoring to original Node version..."

# 恢复原来的版本
nvm use $current_nvm

echo "Prebuilt binaries saved in prebuilds/ directory"
ls -la prebuilds/
