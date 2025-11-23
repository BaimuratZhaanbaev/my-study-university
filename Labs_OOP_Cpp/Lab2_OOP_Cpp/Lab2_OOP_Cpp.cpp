#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


using ClientId = uint64_t;
using Months = int;

	
enum class SubscriptionType : Months {
	OneMonth = 1,
	ThreeMonths = 3,
	SixMonths = 6,
	Year = 12
};


class Client {
public:
	// === Конструкторы ===	
	Client();
	Client(
		std::string full_name,
		std::string card_number,
		SubscriptionType subscription_duration = SubscriptionType::OneMonth
	);

	// === Деструктор ===
	~Client();

	// === Геттеры ===
	inline const std::string& full_name() const noexcept;
	inline const std::string& card_number() const noexcept;
	inline SubscriptionType subscription_type() const noexcept;
	inline Months total_usage_months() const noexcept;

	// === Сеттеры ===
	inline void full_name(std::string name);
	inline void card_number(std::string number);
	inline void set_subscription_type(SubscriptionType type);
	inline void total_usage_months(Months usage_months);

	// === Методы ===
	void print(std::ostream& os = std::cout) const;

private:
	// === Основные данные клиента ===
	std::string full_name_{};          // ФИО
	std::string card_number_{};        // номер дисконтной карты

	// === Абонемент ===
	// Cрок абонемента (1, 3, 6 или 12 месяцев),
	SubscriptionType subscription_type_{SubscriptionType::OneMonth};

	// === Статистика ===
	Months total_usage_months_{0};   // общий срок пользования (накопительно)

	// === Дополнительно (если нужно) ===
	// ClientId id_;                   // внутренний ID (если есть)
	// Date registration_date_;        // дата регистрации
	// bool is_active_ = true;         // активен ли сейчас
};


class SportsClubClients {
public:
	// === Конструкторы ===	
	SportsClubClients();
	SportsClubClients(std::initializer_list<Client> initial_clients);

	// === Деструктор ===
	~SportsClubClients();

	// === Перегрузки ===
	SportsClubClients& operator+=(const Client& client);
	bool operator!() const;
	friend bool operator==(const SportsClubClients& a, const SportsClubClients& b);

	// === Методы ===
	void add_client(std::unique_ptr<Client> client);
	bool remove_by_card_number(const std::string& card_number);
	void print(std::ostream& os = std::cout) const;
	void sort_by_total_usage();
	std::vector<const Client*> find_by_subscription_duration(SubscriptionType duration) const;


	
private:
	// === Поля ===
	std::vector<std::unique_ptr<Client>> clients_{};   // динамический список клиентов
};


// --- Реализация класса Client ---

Client::Client()
	: full_name_()
	, card_number_()
	, subscription_type_(SubscriptionType::OneMonth)
	, total_usage_months_(0)
{
	std::cout << "Конструктор по умолчанию класса Client" << std::endl;
}

Client::Client(
	std::string full_name,
	std::string card_number,
	SubscriptionType subscription_duration
)
	: full_name_(std::move(full_name))
    , card_number_(std::move(card_number)) 
    , subscription_type_(subscription_duration)
    , total_usage_months_(0)
{
	// валидация
	if (subscription_duration != SubscriptionType::OneMonth &&
		subscription_duration != SubscriptionType::ThreeMonths &&
		subscription_duration != SubscriptionType::SixMonths &&
		subscription_duration != SubscriptionType::Year) {
		throw std::invalid_argument("Неверная продолжительность подписки!");
	}
	std::cout << "Конструктор с параметрами класса Client для" << full_name_ << std::endl;
}

Client::~Client() {
	std::cout << "Деструктор для: " << full_name_ << std::endl;
}

inline const std::string& Client::full_name() const noexcept {
	std::cout << "Геттер full_name" << std::endl;
	return full_name_;
}

inline const std::string& Client::card_number() const noexcept { 
    std::cout << "Геттер card_number" << std::endl;
	return card_number_;
}

inline SubscriptionType Client::subscription_type() const noexcept { 
	std::cout << "Геттер subscription_type" << std::endl;
	return subscription_type_;
}

inline Months Client::total_usage_months() const noexcept { 
	std::cout << "Геттер total_usage_months" << std::endl;
	return total_usage_months_;
}

inline void Client::full_name(std::string name) {
	full_name_ = name;
	std::cout << "Сеттер full_name" << std::endl;
}

inline void Client::card_number(std::string number) {
	card_number_ = number;
	std::cout << "Сеттер card_number" << std::endl;
}

inline void Client::set_subscription_type(SubscriptionType type) {
	subscription_type_ = type;
	std::cout << "Сеттер set_subscription_type" << std::endl;
}

inline void Client::total_usage_months(Months usage_months) {
	total_usage_months_ = usage_months;
	std::cout << "Сеттер total_usage_months" << std::endl;
}

void Client::print(std::ostream& os) const {
	os << "ФИО: " << full_name_ << ", Карта: " << card_number_ << ", Абонемент: "
		<< static_cast<int>(subscription_type_) << " мес., Общий срок: "
		<< total_usage_months_ << " мес.\n";
}

// --- Реализация класса SportsClubClients ---

SportsClubClients::SportsClubClients() : clients_(std::vector<std::unique_ptr<Client>>{}) {
	std::cout << "Конструктор по умолчанию класса SportsClubClients" << std::endl;
}

SportsClubClients::SportsClubClients(std::initializer_list<Client> initial_clients) {
	for (const auto& client : initial_clients) {
		clients_.emplace_back(std::make_unique<Client>(client));  // Копируем и выделяем память динамически
	}
	std::cout << "Конструктор с параметрами класса SportsClubClients" << std::endl;
}

SportsClubClients::~SportsClubClients() {
	std::cout << "Деструктор класса SportsClubClients" << std::endl;
}

SportsClubClients& SportsClubClients::operator+=(const Client& client) {
	std::cout << "Вызов бинарной операции" << std::endl;
	add_client(std::make_unique<Client>(client));
	return *this;
}

bool SportsClubClients::operator!() const {
	std::cout << "Вызов унарной операции" << std::endl;
	return clients_.empty();
}

bool operator==(const SportsClubClients& a, const SportsClubClients& b) {
	std::cout << "Вызов глобальной операции не члена класса" << std::endl;
	if (a.clients_.size() != b.clients_.size()) return false;
	for (size_t i = 0; i < a.clients_.size(); ++i) {
		if (a.clients_[i]->card_number() != b.clients_[i]->card_number()) return false;
	}
	return true;
}

void SportsClubClients::add_client(std::unique_ptr<Client> client) {
	std::cout << "Добавление клиента в конец списока" << std::endl;
	clients_.push_back(std::move(client));  // Передаём владение
}

bool SportsClubClients::remove_by_card_number(const std::string& card_number) {
	std::cout << "Удаление клиента по номеру дисконтной карты" << std::endl;
	auto it = std::find_if(clients_.begin(), clients_.end(),
		[&card_number](const auto& c) { return c->card_number() == card_number; });
	if (it != clients_.end()) {
		clients_.erase(it);
		return true;
	}
	return false;
}

void SportsClubClients::print(std::ostream& os) const {
	std::cout << "Вывод всех клиентов на экран" << std::endl;
	for (const auto& client : clients_) {
		client->print(os);
	}
}

void SportsClubClients::sort_by_total_usage() {
	std::cout << "Сортировка по общему сроку пользования" << std::endl;
	std::sort(clients_.begin(), clients_.end(),
		[](const auto& a, const auto& b) {
			return a->total_usage_months() < b->total_usage_months();
		});
}

std::vector<const Client*> SportsClubClients::find_by_subscription_duration(SubscriptionType duration) const {
	std::cout << "Поиск клиентов с одинаковым сроком абонемента" << std::endl;
	std::vector<const Client*> result;
	for (const auto& client : clients_) {
		if (client->subscription_type() == duration) {
			result.push_back(client.get());
		}
	}
	return result;
}


int main() {
	std::setlocale(LC_ALL, "Russian");
	std::cout << "=== Создание клуба с initializer_list ===\n";
	SportsClubClients club = {
		{"Иванов Иван", "A001", SubscriptionType::Year},
		{"Петрова Анна", "B002", SubscriptionType::SixMonths}
	};

	std::cout << "=== Добавление клиента динамически ===\n";
	auto new_client = std::make_unique<Client>("Сидоров Петр", "C003", SubscriptionType::ThreeMonths);
	new_client->total_usage_months(5);
	club.add_client(std::move(new_client));

	std::cout << "=== Вывод всех клиентов ===\n";
	club.print();

	std::cout << "=== Сортировка по общему сроку ===\n";
	club.sort_by_total_usage();
	club.print();

	std::cout << "=== Удаление по номеру карты ===\n";
	club.remove_by_card_number("B002");
	club.print();

	std::cout << "=== Поиск по абонементу ===\n";
	auto found = club.find_by_subscription_duration(SubscriptionType::ThreeMonths);
	for (const auto* c : found) {
		c->print();
	}

	std::cout << "=== Демонстрация перегрузок ===\n";
	SportsClubClients club2;
	Client temp("Козлова Мария", "D004", SubscriptionType::OneMonth);
	club2 += temp;
	club2.print();

	std::cout << "Клуб2 пуст? " << (!club2 ? "Да" : "Нет") << std::endl;

	SportsClubClients club3;
	Client temp2("Козлова Мария", "D004", SubscriptionType::OneMonth);
	club3 += temp2;
	std::cout << "club2 == club3? " << (club2 == club3 ? "Да" : "Нет") << std::endl;

	std::cout << "=== Конец main, вызов деструкторов ===\n";
	return 0;
}
