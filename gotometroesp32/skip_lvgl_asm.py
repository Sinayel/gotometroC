Import("env")

def skip_asm(node):
    if "lvgl" in node.get_path():
        return None   # ignore ce fichier
    return node

env.AddBuildMiddleware(skip_asm, "*.S")