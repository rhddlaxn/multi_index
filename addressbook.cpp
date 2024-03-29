#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT addressbook: public contract{
    public:
        using contract::contract;

        ACTION findage(uint64_t age) {
            address_index addresses(get_self(), get_self().value);
            auto forsecondary = addresses.get_index<"byage"_n>();
            auto itr = forsecondary.require_find(age, "no age");
            print(itr -> last_name, " ", itr->age);
        }
     
        
        ACTION upsert(name user, std::string first_name, std::string last_name, uint64_t age) {
            require_auth(user);
           
            address_index forUpsert(get_self(), get_self().value);
            auto itr = forUpsert.find(user.value); //auto 뒤에 오는 변수의 자료형을 알아서 바꿔주어 자료형을 안써줘도됨
            
            if(itr == forUpsert.end() )
            {
            forUpsert.emplace(user, [&](auto& row){
                row.user = user;
                row.first_name = first_name;
                row.last_name = last_name;
                row.age = age;
            })
            ;print("upsert success");
            }
            else { 
                forUpsert.modify(itr, user, [&](auto& row){
                    row.user = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.age = age;
                });
                print("modify success");
            }
            
        }
        ACTION erase(name user) {
            require_auth(user);
            
            address_index forErase(get_self(), get_self().value);
            auto itr = forErase.require_find(user.value, "no account");
            forErase.erase(itr);

            print("erase success");
            }
    private:
        // struct [[eosio::table]] person = TABLE person 로 정의한것
        TABLE person{
            name user;
            std::string first_name;
            std::string last_name;
            uint64_t age;
            
            uint64_t primary_key() const { return user.value;}
            uint64_t by_age() const {return age;}
        };

        typedef multi_index<"peopletwo"_n, person,
        indexed_by<"byage"_n, const_mem_fun<person, uint64_t, &person::by_age>> > address_index;
};
