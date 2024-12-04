import random

# 域名后缀列表
domain_suffixes = ['.com', '.net', '.org', '.edu', '.io', '.co.uk']

# 随机生成域名
def generate_domain():
    domain_parts = [random_word() for _ in range(2)]  # 生成两个随机单词作为域名部分
    domain = '.'.join(domain_parts) + random.choice(domain_suffixes)  # 添加域名后缀
    return domain

# 生成一个随机单词
def random_word():
    letters = 'abcdefghijklmnopqrstuvwxyz'
    return ''.join(random.choice(letters) for _ in range(random.randint(3, 8)))  # 随机单词长度在3到8个字符之间

# 随机生成IP地址
def generate_ip():
    return '.'.join(str(random.randint(0, 255)) for _ in range(4))

# 生成域名和IP对，并写入文件
def write_domains_and_ips(filename, num_entries):
    with open(filename, 'w') as file:
        for _ in range(num_entries):
            domain = generate_domain()
            ip = generate_ip()
            file.write(f"{domain} {ip}\n")

# 指定要生成的条目数量
num_entries = 100  # 例如，生成100个域名和IP对
write_domains_and_ips('domain_and_ip', num_entries)